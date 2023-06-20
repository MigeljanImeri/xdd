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
/*
 * This file contains the subroutines that perform various initialization
 * functions with respect to memory such as locking and unlocking pages.
 */
#include "xint.h"

/*----------------------------------------------------------------------------*/
void
xdd_lock_memory(unsigned char *bp, uint32_t bsize, char *sp) {
	int32_t status; /* status of a system call */

	// If the nomemlock option is set then do not lock memory
	if (xgp->global_options & GO_NOMEMLOCK)
		return;

	if (getuid() != 0) {
		 if (xgp->global_options&GO_REALLYVERBOSE) {
			fprintf(xgp->errout,"(PID %d) %s: You must run as superuser to lock memory for %s\n",
				getpid(),xgp->progname, sp);
		 }
		return;
	}
	status = mlock((char *)bp, bsize);
	if (status < 0) {
			fprintf(xgp->errout,"(PID %d) %s: Could not lock %d bytes of memory for %s\n",
				getpid(),xgp->progname,bsize,sp);
		perror("Reason");
	}
} /* end of xdd_lock_memory() */
/*----------------------------------------------------------------------------*/
void
xdd_unlock_memory(unsigned char *bp, uint32_t bsize, char *sp) {
	int32_t status; /* status of a system call */

	// If the nomemlock option is set then do not unlock memory because it was never locked
	if (xgp->global_options & GO_NOMEMLOCK)
		return;
	if (getuid() != 0) {
		return;
	}
	status = munlock((char *)bp, bsize);
	if (status < 0) {
			fprintf(xgp->errout,"(PID %d) %s: Could not unlock memory for %s\n",
				getpid(),xgp->progname,sp);
		perror("Reason");
	}
} /* end of xdd_unlock_memory() */
