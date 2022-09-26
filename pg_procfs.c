/*-------------------------------------------------------------------------
 *  
 * pg_procfs is a PostgreSQL extension which allows to display 
 * /procfs data using SQL. 
 * 
 * This program is open source, licensed under the PostgreSQL license.
 * For license terms, see the LICENSE file.
 *          
 * Copyright (c) 2022, Pierre Forstmann.
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group 
 *            
 *-------------------------------------------------------------------------
*/
#include "postgres.h"

#include "utils/guc.h"
#include "utils/fmgrprotos.h"
#include "fmgr.h"
#include "funcapi.h"
#include "miscadmin.h"
#include "pgtime.h"
#include "utils/timestamp.h"
#include "pgstat.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utils/builtins.h"

#if PG_VERSION_NUM < 130000
#include "catalog/pg_type.h"
#endif

#define PG_PROCFS_MAX_LINE_SIZE	32768	

PG_MODULE_MAGIC;

/*---- Function declarations ----*/

void		_PG_init(void);
void		_PG_fini(void);

PG_FUNCTION_INFO_V1(pg_read);
PG_FUNCTION_INFO_V1(pg_procfs);
static Datum pg_read_internal(const char *filename);
static Datum pg_procfs_internal(FunctionCallInfo fcinfo);

/*---- Variable declarations ----*/

static text *g_result;

/*
 * structure to access /proc  data 
 */
struct	data {
	text	*data;
	int	char_count;
	int	line_count;
	int	i; 
	int	max_line_size;
	char	*p;
	int	first_newline_position;
} g_data;


/*
 * Module load callback
 */
void
_PG_init(void)
{


	elog(DEBUG5, "pg_procfs:_PG_init():entry");

	/* get the configuration */

	/* currently no GUC parameter */

	elog(DEBUG5, "pg_procfs:_PG_init():exit");
}

/*
 *  Module unload callback
 */
void
_PG_fini(void)
{
	elog(DEBUG5, "pg_procfs:_PG_fini():entry");

	elog(DEBUG5, "pg_procfs:_PG_fini():exit");
}
/* --- ---- */


static void data_start(text *p_result)
{
	g_data.data = p_result;	
	g_data.char_count = 0;
       	g_data.p = VARDATA(p_result);
	g_data.line_count = 0;
       	g_data.i = 0;
	g_data.max_line_size = 0;
	g_data.first_newline_position = -1;

}

static bool data_has_more()
{
	return  g_data.char_count < VARSIZE(g_data.data);

}

static void data_next()
{
        g_data.char_count++;
	g_data.i++;
}

static	char data_get()
{
	return *(g_data.p + g_data.char_count);
}

static	void data_incr_line_count()
{
	g_data.line_count++;
}

static void data_set_max_line_size()
{
	g_data.max_line_size = g_data.i;
}

static bool data_index_gt_max_line_size()
{
	return g_data.i > g_data.max_line_size;
}

static int data_get_index()
{
	return g_data.i;
}

static void data_reset_index()
{
	g_data.i = 0;
}

static int data_get_char_count()
{
	return g_data.char_count;
}

static int data_get_line_count()
{
	return g_data.line_count;
}

static int data_get_max_line_size()
{
	return g_data.max_line_size;
}

static void data_set_first_newline_position (int pos)
{
	g_data.first_newline_position = pos;
}

/* --- ---- */

Datum pg_read(PG_FUNCTION_ARGS)
{
   char *filename;

   filename = PG_GETARG_CSTRING(0); 
   return (pg_read_internal(filename));
}

static Datum pg_read_internal(const char *filename)
{
	
	PGFunction	func;
	text		*lfn;	
	struct stat	stat_buf;
	int		rc;
	text		*result;
	char		c;
	int		first_newline_position = 0;

	rc = stat(filename, &stat_buf);
	if (rc != 0)
		elog(ERROR, "pg_read_internal: stat failed on %s", filename);

	elog(DEBUG1, "pg_read_internal: %s has %ld bytes", filename, stat_buf.st_size); 

	lfn = (text *) palloc(strlen(filename) + VARHDRSZ);
	memcpy(VARDATA(lfn), filename, strlen(filename));
	SET_VARSIZE(lfn, strlen(filename) + VARHDRSZ);

#if PG_VERSION_NUM > 110000
	func = pg_read_file_all;
#else
	func = pg_read_file;
#endif
	result =  (text *)DirectFunctionCall1(func, (Datum)lfn);

	/*
	 * check returned data
	 */

	for (data_start(result);  data_has_more(); data_next())
	{
		c = data_get();
		if (c == '\n')
		{
			  if (first_newline_position == 0)
				  first_newline_position = data_get_index();
			  data_incr_line_count();
			  if (data_index_gt_max_line_size())
			  	data_set_max_line_size();
			  data_reset_index();
		}
	
	}

	data_set_first_newline_position(first_newline_position);

	elog(DEBUG1, "pg_read_internal: checked %d characters in %d lines (longest=%d)", data_get_char_count(), data_get_line_count(), data_get_max_line_size());
	g_result = result;

	return (Datum)0;

}

Datum pg_procfs(PG_FUNCTION_ARGS)
{
   char *filename;

   filename = PG_GETARG_CSTRING(0); 
   elog(DEBUG1, "pg_procfs: filename=%s", filename); 

   if (strstr(filename, "/proc") == NULL)
	   elog(ERROR, "pg_procfs: file name %s does not belong to /proc", filename);

   pg_read_internal(filename);
   return (pg_procfs_internal(fcinfo));
}


static Datum pg_procfs_internal(FunctionCallInfo fcinfo)
{


	ReturnSetInfo 	*rsinfo = (ReturnSetInfo *) fcinfo->resultinfo;
	bool		randomAccess;
	TupleDesc	tupdesc;
	Tuplestorestate *tupstore;
	AttInMetadata	 *attinmeta;
	MemoryContext 	oldcontext;

	int		i;
	int		c;

	/* check to see if caller supports us returning a tuplestore */
	if (rsinfo == NULL || !IsA(rsinfo, ReturnSetInfo))
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("set-valued function called in context that cannot accept a set")));
	if (!(rsinfo->allowedModes & SFRM_Materialize))
		ereport(ERROR,
				(errcode(ERRCODE_SYNTAX_ERROR),
				 errmsg("materialize mode required, but it is not allowed in this context")));

	/* The tupdesc and tuplestore must be created in ecxt_per_query_memory */
	oldcontext = MemoryContextSwitchTo(rsinfo->econtext->ecxt_per_query_memory);
#if PG_VERSION_NUM <= 120000
	tupdesc = CreateTemplateTupleDesc(2, false);
#else
	tupdesc = CreateTemplateTupleDesc(2);
#endif
	TupleDescInitEntry(tupdesc, (AttrNumber) 1, "lineno", INT4OID, -1, 0);
	TupleDescInitEntry(tupdesc, (AttrNumber) 2, "data", TEXTOID, -1, 0);

	randomAccess = (rsinfo->allowedModes & SFRM_Materialize_Random) != 0;
	tupstore = tuplestore_begin_heap(randomAccess, false, work_mem);
	rsinfo->returnMode = SFRM_Materialize;
	rsinfo->setResult = tupstore;
	rsinfo->setDesc = tupdesc;

	MemoryContextSwitchTo(oldcontext);

	attinmeta = TupleDescGetAttInMetadata(tupdesc);

	for (data_start(g_result), i=0 ;data_has_more();data_next(), i++)
        {
		char		buf_v1[20];
		char		buf_v2[PG_PROCFS_MAX_LINE_SIZE];
		char 		*values[2];
		HeapTuple	tuple;

		c = data_get();
		buf_v2[i] = c;
		
		if ( data_get_index() > PG_PROCFS_MAX_LINE_SIZE - 1)
			elog(ERROR, "pg_procfs_internal: log line %d larger than %d", data_get_line_count() + 1, PG_PROCFS_MAX_LINE_SIZE);

                if (c == '\n')
                {
			sprintf(buf_v1, "%d", data_get_line_count());	
			buf_v2[i] = '\0';
			data_incr_line_count();
			i = -1;

			values[0] = buf_v1;
			values[1] = buf_v2;
			tuple = BuildTupleFromCStrings(attinmeta, values);
			tuplestore_puttuple(tupstore, tuple);
                }
        }

	return (Datum)0;

}

