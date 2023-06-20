/*
 * XDD - a data movement and benchmarking toolkit
 *
 * Copyright (C) 1992-23 I/O Performance, Inc.
 * Copyright (C) 2009-23 UT-Battelle, LLC
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License version 2, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 *//*
 * This file contains the subroutines that support the Target threads.
 */
#include "xint.h"

/*----------------------------------------------------------------------------*/
/* xdd_target_thread_cleanup() - Perform termination processing of this
 * this Target thread.
 * Return Values: 0 is good, -1 indicates an error but what are ya gonna do?
 *
 */
void
xdd_target_thread_cleanup(target_data_t *tdp) {
	worker_data_t	*wdp;		// Pointer to a Worker Thread Data Struct
	int rc;

	wdp = tdp->td_next_wdp;
	while (wdp) {
		wdp->wd_task.task_request = TASK_REQ_STOP;
		tdp->td_occupant.occupant_type |= XDD_OCCUPANT_TYPE_CLEANUP;
		// Release this Worker Thread
		xdd_barrier(&wdp->wd_thread_targetpass_wait_for_task_barrier,&tdp->td_occupant,0);

		// get the next Worker in this chain
		wdp = wdp->wd_next_wdp;
	}

    /* We need to close all file descriptors opened by worker threads in islocal e2e connection */
    if (tdp->td_planp->plan_options & PLAN_ENDTOEND_LOCAL) {
        wdp = tdp->td_next_wdp;
        while (wdp) {
            if (wdp->wd_e2ep && wdp->wd_e2ep->e2e_sd != -1) {
                close(wdp->wd_e2ep->e2e_sd);
                wdp->wd_e2ep->e2e_sd = -1;
            }
            // Get the next worker in the chain
            wdp = wdp->wd_next_wdp;
        }
    }

    if (tdp->td_target_options & TO_DELETEFILE) {
        if (tdp->td_planp->plan_options & PLAN_ENDTOEND_LOCAL) {
            xdd_targetpass_e2e_remove_islocal(tdp);
        } else {
		    unlink(tdp->td_target_full_pathname);
	    }
    }

   /* On e2e XNI, part of cleanup includes closing the source side */
   if ((TO_ENDTOEND & tdp->td_target_options) &&
       (PLAN_ENABLE_XNI & tdp->td_planp->plan_options)) {
       xni_close_connection(&tdp->td_e2ep->xni_td_conn);
   }

    /* If the target data pattern was allocated, it must be free'd */
    if (tdp->td_dpp) {
        if (tdp->td_dpp->data_pattern_options & (DP_FILE_PATTERN | DP_WHOLEFILE_PATTERN)) {
            /* If the file target opend a file for the data pattern we must close
            * the files and clean up the data_pattern buffer
            */
            close(tdp->dpp_fd);
            free(tdp->td_dpp->data_pattern);
        }
        free(tdp->td_dpp);
    }

	/* On non e2e, close the descriptor */
	if (!(TO_ENDTOEND & tdp->td_target_options)) {
		rc = close(tdp->td_file_desc);
		// Check the status of the CLOSE operation to see if it worked
		if (rc != 0) {
			fprintf(xgp->errout,"%s: xdd_target_thread_cleanup: ERROR: Could not close target number %d name %s\n",
				xgp->progname,
				tdp->td_target_number,
       		    tdp->td_target_full_pathname);
            fflush(xgp->errout);
            perror("reason");
		}
	}

} // End of xdd_target_thread_cleanup()
