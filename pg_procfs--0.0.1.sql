--
-- pg_procfs--0.0.1.sql
--
DROP FUNCTION IF EXISTS pg_procfs();
---
CREATE FUNCTION pg_procfs(IN filename cstring, OUT line integer, OUT data text) RETURNS SETOF record 
 AS 'pg_procfs.so', 'pg_procfs'
 LANGUAGE C STRICT;
--
