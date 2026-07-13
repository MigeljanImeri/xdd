/*
 * XDD - a data movement and benchmarking toolkit
 *
 * Copyright (C) 1992-2013 I/O Performance, Inc.
 * Copyright (C) 2009-2013 UT-Battelle, LLC
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License version 2, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

#include "xint_plan.h"

#define XDD_PARSE_PHASE1			0x00000001 // Preprocess phase - phase 1
#define XDD_PARSE_PHASE2			0x00000002 // Parse Phase 2 occurs after we know how many targets there really are
#define XDD_PARSE_TARGET_IN			0x00000004 // Target In
#define XDD_PARSE_TARGET_OUT		0x00000008 // Target Out
#define XDD_PARSE_TARGET_FILE		0x00000010 // Target_inout I/O Type of "FILE"
#define XDD_PARSE_TARGET_NETWORK	0x00000020 // Target_inout I/O Type of "NETWORK"

/* The format of the entries in the xdd function table */
typedef int (*func_ptr)(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);

#define XDD_EXT_HELP_LINES 5
struct xdd_func {
	char    *func_name;     /* name of the function */
	char    *func_alt;      /* Alternate name of the function */
    int     (*func_ptr)(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);      /* pointer to the function */
    int     argc;           /* number of arguments */
    char    *help;          /* help string */
    char    *ext_help[XDD_EXT_HELP_LINES];   /* Extented help strings */
	uint32_t flags;			/* Flags for various parsing functions */
};
typedef struct xdd_func xdd_func_t;

#define	XDD_FUNC_INVISIBLE	0x00000001	// When this flag is present then this command will not be displayed with "usage"

// Prototypes required by the parse_table() compilation
// Arguments: plan struct, number of arguments, argument table, flag mask for option types, target condition is applied to
int xddfunc_blocksize(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_bytes(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_combinedout(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_congestion(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_cookie(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_createnewfiles(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_csvout(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_datapattern(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_debug(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_deletefile(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_devicefile(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_dio(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_dryrun(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_endtoend(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_errout(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_extended_stats(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_flushwrite(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_fullhelp(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_heartbeat(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_help(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_id(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_interactive(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_kbytes(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_lockstep(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_looseordering(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_maxall(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_maxerrors(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_max_errors_to_print(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_maxpri(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_mbytes(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_memalign(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_memory_usage(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_minall(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_multipath(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_nobarrier(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_nomemlock(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_noordering(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_noproclock(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
#if HAVE_ENABLE_NUMA
int xddfunc_numactl(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
#endif
int xddfunc_numreqs(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_operationdelay(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_operation(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_ordering(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_output(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_output_format(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_passdelay(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_passes(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_passoffset(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_percentcpu(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_pretruncate(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_processlock(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_processor(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_queuedepth(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_randomize(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_readafterwrite(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_reallyverbose(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_recreatefiles(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_reopen(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_report_threshold(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_restart(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_retry(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_roundrobin(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_runtime(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_rwratio(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_seek(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_setup(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_sgio(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_sharedmemory(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_singleproc(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_startdelay(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_startoffset(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_starttime(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_starttrigger(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_stoponerror(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_stoptrigger(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_serialordering(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_syncio(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_syncwrite(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_target(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_target_inout(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_target_inout_file(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_target_inout_network(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_targetdir(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_targetin(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_targetoffset(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_targets(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_targetstartdelay(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_targetout(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_throttle(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_timelimit(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_timerinfo(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_timeserver(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_timestamp(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_verify(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_unverbose(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_verbose(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_version(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_xni(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_ibdevice(xdd_plan_t *planp, int32_t argc, char *argv[], uint32_t flags, int32_t target);
int xddfunc_invalid_option(int32_t argc, char *argv[], uint32_t flags);
void xddfunc_currently_undefined_option(char *sp);
uint64_t xddfunc_parse_size_with_units(const char *value, const char *type, int* error);
void xddfunc_convert_bytes_to_units(uint64_t number, char *output_buf, size_t output_length);

/*
 * Local variables:
 *  indent-tabs-mode: t
 *  default-tab-width: 4
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 noexpandtab
 */
