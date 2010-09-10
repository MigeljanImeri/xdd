/* Copyright (C) 1992-2010 I/O Performance, Inc. and the
 * United States Departments of Energy (DoE) and Defense (DoD)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program in a file named 'Copying'; if not, write to
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139.
 */
/* Principal Author:
 *      Tom Ruwart (tmruwart@ioperformance.com)
 * Contributing Authors:
 *       Steve Hodson, DoE/ORNL
 *       Steve Poole, DoE/ORNL
 *       Bradly Settlemyer, DoE/ORNL
 *       Russell Cattelan, Digital Elves
 *       Alex Elder
 * Funding and resources provided by:
 * Oak Ridge National Labs, Department of Energy and Department of Defense
 *  Extreme Scale Systems Center ( ESSC ) http://www.csm.ornl.gov/essc/
 *  and the wonderful people at I/O Performance, Inc.
 */
/*
 * This file contains the subroutine that locates an available QThread for
 * a specific target.
 */
#include "xdd.h"

/*----------------------------------------------------------------------------*/
/* xdd_get_next_available_qthread() - This subroutine will scan the list of
 * QThreads and return one of two values:
 *      (1) A pointer to a QThread that is available to be assigned a task
 *      (2) A NULL pointer if this is the Destination Side of an E2E operation
 *          -and- ALL the QThreads have received their EOF packet indicated
 *			by the "pass_complete" variable being set to 1.
 * Under most conditions this subroutine will block until a QThread becomes 
 * available except for the E2E Destination Side case mentioned above. 
 * The input parameter is a pointer to the Target Thread PTDS that owns 
 * the chain of QThreads to be scanned.
 * This subroutine is called by target_pass_loop() or target_pass_e2e_loop().
 */
ptds_t *
xdd_get_next_available_qthread(ptds_t *p) {
	ptds_t		*qp;					// Pointer to a QThread PTDS
	int			status;					// Status of the sem_wait system calls
	int			false_scans;			// Number of times we looked at the QP Chain expecting to find an available QThread but did not
	int			completed_qthreads;		// Number of QThreads that have completed their passes


	if ((p->target_options & TO_SERIAL_ORDERING) || 
		(p->target_options & TO_LOOSE_ORDERING)) { // Serial or Loose Ordering requires us to wait for a "specific" QThread to become available
		qp = p->next_qthread_to_use;
		if (qp->next_qp) // Is there another QThread on this chain after the current one?
			p->next_qthread_to_use = qp->next_qp; // point to the next QThread in the chain
		else p->next_qthread_to_use = p->next_qp; // else point to the first QThread in the chain
		// Wait for this specific QThread to become available
		p->my_current_state |= CURRENT_STATE_WAITING_THIS_QTHREAD_AVAILABLE;
		status = sem_wait(&qp->sem_this_qthread_is_available);
		if (status) {
			fprintf(xgp->errout,"%s: xdd_get_next_available_qthread: Target %d: WARNING: Bad status from sem_wait on this_qthread_is_available semaphore: status=%d, errno=%d\n",
				xgp->progname,
				p->my_target_number,
				status,
				errno);
		}
		p->my_current_state &= ~CURRENT_STATE_WAITING_THIS_QTHREAD_AVAILABLE;

		pthread_mutex_lock(&qp->mutex_this_qthread_is_available);
		// If this QThread is available *and* pass_complete has NOT been set then we can assign this QThread
		if ((qp->this_qthread_is_available) && (qp->pass_complete == 0))  { 
			qp->this_qthread_is_available = 0; // Mark this QThread Unavailable
			pthread_mutex_unlock(&qp->mutex_this_qthread_is_available);
		} else {
			pthread_mutex_unlock(&qp->mutex_this_qthread_is_available);
			qp = 0;
		}

		// If the QThread is available and it has not processed its end-of-pass then return the correct QThread pointer
		// Otherwise, we will return 0 - which should not happen but you never know...
		return(qp);
	} 
	// No Serial Ordering - just wait for any QThread to become available
	false_scans = 0;
	qp = 0;
	while (qp == 0) {
		p->my_current_state |= CURRENT_STATE_WAITING_ANY_QTHREAD_AVAILABLE;
		status = sem_wait(&p->sem_any_qthread_available);
		if (status) {
			fprintf(xgp->errout,"%s: xdd_get_next_available_qthread: Target %d: WARNING: Bad status from sem_post on any_qthread_available semaphore: status=%d, errno=%d\n",
				xgp->progname,
				p->my_target_number,
				status,
				errno);
		}
		p->my_current_state &= ~CURRENT_STATE_WAITING_ANY_QTHREAD_AVAILABLE;
		// At this point we know that at least ONE QThread has indicated that it is available. 
		// The following WHILE loop looks for that "available" QThread and takes the first one it finds
		// that is available and does NOT have its "pass_complete" variable set.
		// Any QThread that is "available" -and- has its "pass_complete" variable set to 1 is not 
		// able to be used for a task. Such a QThread is considered to have "completed" and will be 
		// counted as a "completed_qthread". When the number of "completed_qthreads" is equal to the
		// total number of QThreads for this Target Thread, then a NULL QThread pointer is returned
		// to the caller indicating that all QThreads have completed this pass. 

		// Get the first QThread pointer from this Target
		qp = p->next_qp;
		completed_qthreads = 0;
		while (qp) {
			pthread_mutex_lock(&qp->mutex_this_qthread_is_available);
			// If this QThread is available *and* pass_complete has NOT been set then we can assign this QThread
			if ((qp->this_qthread_is_available) && (qp->pass_complete == 0))  { 
				qp->this_qthread_is_available = 0; // Mark this QThread Unavailable
				pthread_mutex_unlock(&qp->mutex_this_qthread_is_available);
				break; // qp now points to the QThread to use
			}

			if ((qp->this_qthread_is_available) && (qp->pass_complete == 1))  // This QThread has completed its pass
				completed_qthreads++;

			// Release the "this_qthread_is_available" lock 
			pthread_mutex_unlock(&qp->mutex_this_qthread_is_available);

			// Get the next QThread Pointer 
			qp = qp->next_qp;
		} // End of WHILE (qp) that scans the QThread Chain looking for the QThread that said it was Available

		// At this point:
		//    The variable "qp" is either 0 or it points to a valid QThread  
		//    If "qp" is non-zero then we will break out of this WHILE loop and return it to the caller
		//    If "qp" is zero *and* the number of QThreads that have completed equals the Queue Depth, that means
		//        that this is the last QThread to complete in which case we need to return 0 as well. This is the 
		//        ONLY time we return 0.
		//    In any event, the "mutex_this_qthread_is_available" will be unlocked
		if ((qp == 0)  && (completed_qthreads == p->queue_depth)) { // When there are no available QThreads because they have all completed their passes then return NULL
			return(0);
		} 
	} // END of WHILE (qp == 0)
	return(qp);
} // End of xdd_get_next_available_qthread()
