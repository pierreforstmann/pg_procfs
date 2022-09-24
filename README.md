# pg_procfs
PostgreSQL extension to display /proc FS data from SQL 


# Installation
## Compiling

This module can be built using the standard PGXS infrastructure. For this to work, the `pg_config` program must be available in your $PATH:
  
`git clone https://github.com/pierreforstmann/pg_procfs.git` <br>
`cd pg_procfs` <br>
`make` <br>
`make install` <br>

This extension has been validated with PostgreSQL 10, 11, 12, 13 and 14.

## PostgreSQL setup

Extension must loaded at server level with `shared_preload_libraries` parameter.

# Usage

## Example

Add in `postgresql.conf`:

`shared_preload_libraries = 'pg_procfs'` <br>

Run: <br>
`create extension pg_profcs`;

To display some /proc FS data:<br>
`select * from pg_procfs('/proc/version');`<br>

