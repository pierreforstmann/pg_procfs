# pg_procfs
PostgreSQL extension to display /proc FS data from SQL 


# Installation
## Compiling

This module can be built using the standard PGXS infrastructure. For this to work, the `pg_config` program must be available in your $PATH:
  
`git clone https://github.com/pierreforstmann/pg_procfs.git` <br>
`cd pg_procfs` <br>
`make` <br>
`make install` <br>

This extension has been validated with PostgreSQL 14.

## PostgreSQL setup

Extension must loaded at server level with `shared_preload_libraries` parameter.

# Usage

## Example

Add in `postgresql.conf`:

`shared_preload_libraries = 'pg_procfs'` <br>

Run: <br>
`create extension pg_profcs`;

To display some /proc FS data:<br>
```
select * from pg_procfs('/proc/version');
 line |                                                                                     message                                          
                                            
------+--------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------
    0 | Linux version 4.18.0-372.19.1.el8_6.x86_64 (mockbuild@49c5e54ed716424c9ae8c1a3d1fef96f) (gcc version 8.5.0 20210514 (Red Hat 8.5.0-10
) (GCC)) #1 SMP Tue Aug 2 13:42:59 EDT 2022
(1 row)
```

