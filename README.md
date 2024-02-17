# pg_procfs
PostgreSQL extension to display /proc FS data from SQL 


# Installation
## Compiling

This module can be built using the standard PGXS infrastructure. For this to work, the `pg_config` program must be available in your $PATH:
  
`git clone https://github.com/pierreforstmann/pg_procfs.git` <br>
`cd pg_procfs` <br>
`make` <br>
`make install` <br>

This extension has been validated with PostgreSQL 11, 12, 13, 14, 15 and 16.

It is not supported with PostgreSQL 10 due to following error message:
`ERROR:  absolute path not allowed`

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
# select * from pg_procfs('/proc/vmstat');
 line |                 data                 
------+--------------------------------------
    0 | nr_free_pages 2463660
    1 | nr_zone_inactive_anon 431107
    2 | nr_zone_active_anon 3212
    3 | nr_zone_inactive_file 414735
    4 | nr_zone_active_file 282774
    5 | nr_zone_unevictable 16
    6 | nr_zone_write_pending 103
    7 | nr_mlock 16
    8 | nr_bounce 0
    9 | nr_zspages 0
   10 | nr_free_cma 0
   11 | numa_hit 56076896
   12 | numa_miss 0
   13 | numa_foreign 0
   14 | numa_interleave 46068
   15 | numa_local 56076896
   16 | numa_other 0
   17 | nr_inactive_anon 431107
   18 | nr_active_anon 3212
   19 | nr_inactive_file 414735
   20 | nr_active_file 282774
   21 | nr_unevictable 16
   22 | nr_slab_reclaimable 47407
   23 | nr_slab_unreclaimable 43267
   24 | nr_isolated_anon 0
   25 | nr_isolated_file 0
   26 | workingset_nodes 0
   27 | workingset_refault_anon 0
   28 | workingset_refault_file 0
   29 | workingset_activate_anon 0
   30 | workingset_activate_file 0
   31 | workingset_restore_anon 0
   32 | workingset_restore_file 0
   33 | workingset_nodereclaim 0
   34 | nr_anon_pages 399711
   35 | nr_mapped 148924
   36 | nr_file_pages 730216
   37 | nr_dirty 103
   38 | nr_writeback 0
   39 | nr_writeback_temp 0
   40 | nr_shmem 32707
   41 | nr_shmem_hugepages 0
   42 | nr_shmem_pmdmapped 0
   43 | nr_file_hugepages 0
   44 | nr_file_pmdmapped 0
   45 | nr_anon_transparent_hugepages 298
   46 | nr_vmscan_write 0
   47 | nr_vmscan_immediate_reclaim 0
   48 | nr_dirtied 476869
   49 | nr_written 299252
   50 | nr_kernel_misc_reclaimable 0
   51 | nr_foll_pin_acquired 0
   52 | nr_foll_pin_released 0
   53 | nr_kernel_stack 18416
   54 | nr_page_table_pages 15792
   55 | nr_swapcached 0
   56 | nr_dirty_threshold 1247540
   57 | nr_dirty_background_threshold 311504
   58 | pgpgin 2282707
   59 | pgpgout 1599344
   60 | pswpin 0
   61 | pswpout 0
   62 | pgalloc_dma 512
   63 | pgalloc_dma32 1025
   64 | pgalloc_normal 68905196
   65 | pgalloc_movable 0
   66 | allocstall_dma 0
   67 | allocstall_dma32 0
   68 | allocstall_normal 0
   69 | allocstall_movable 0
   70 | pgskip_dma 0
   71 | pgskip_dma32 0
   72 | pgskip_normal 0
   73 | pgskip_movable 0
   74 | pgfree 71370775
   75 | pgactivate 846943
   76 | pgdeactivate 0
   77 | pglazyfree 0
   78 | pgfault 57505697
   79 | pgmajfault 2603
   80 | pglazyfreed 0
   81 | pgrefill 0
   82 | pgsteal_kswapd 0
   83 | pgsteal_direct 0
   84 | pgscan_kswapd 0
   85 | pgscan_direct 0
   86 | pgscan_direct_throttle 0
   87 | pgscan_anon 0
   88 | pgscan_file 0
   89 | pgsteal_anon 0
   90 | pgsteal_file 0
   91 | zone_reclaim_failed 0
   92 | pginodesteal 0
   93 | slabs_scanned 0
   94 | kswapd_inodesteal 0
   95 | kswapd_low_wmark_hit_quickly 0
   96 | kswapd_high_wmark_hit_quickly 0
   97 | pageoutrun 0
   98 | pgrotated 167
   99 | drop_pagecache 0
  100 | drop_slab 0
  101 | oom_kill 0
  102 | numa_pte_updates 0
  103 | numa_huge_pte_updates 7
  104 | numa_hint_faults 0
  105 | numa_hint_faults_local 0
  106 | numa_pages_migrated 0
  107 | pgmigrate_success 0
  108 | pgmigrate_fail 0
  109 | compact_migrate_scanned 0
  110 | compact_free_scanned 0
  111 | compact_isolated 0
  112 | compact_stall 0
  113 | compact_fail 0
  114 | compact_success 0
  115 | compact_daemon_wake 0
  116 | compact_daemon_migrate_scanned 0
  117 | compact_daemon_free_scanned 0
  118 | htlb_buddy_alloc_success 0
  119 | htlb_buddy_alloc_fail 0
  120 | unevictable_pgs_culled 39822
  121 | unevictable_pgs_scanned 0
  122 | unevictable_pgs_rescued 4477
  123 | unevictable_pgs_mlocked 4494
  124 | unevictable_pgs_munlocked 4478
  125 | unevictable_pgs_cleared 0
  126 | unevictable_pgs_stranded 0
  127 | thp_fault_alloc 4924
  128 | thp_fault_fallback 0
  129 | thp_fault_fallback_charge 0
  130 | thp_collapse_alloc 369
  131 | thp_collapse_alloc_failed 0
  132 | thp_file_alloc 0
  133 | thp_file_fallback 0
  134 | thp_file_fallback_charge 0
  135 | thp_file_mapped 0
  136 | thp_split_page 0
  137 | thp_split_page_failed 0
  138 | thp_deferred_split_page 494
  139 | thp_split_pmd 1106
  140 | thp_split_pud 0
  141 | thp_zero_page_alloc 1
  142 | thp_zero_page_alloc_failed 0
  143 | thp_swpout 0
  144 | thp_swpout_fallback 0
  145 | balloon_inflate 0
  146 | balloon_deflate 0
  147 | balloon_migrate 0
  148 | swap_ra 0
  149 | swap_ra_hit 0
  150 | nr_unstable 0
(151 rows)
```

