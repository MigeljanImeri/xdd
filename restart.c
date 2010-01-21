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
 * Funding and resources provided by:
 * Oak Ridge National Labs, Department of Energy and Department of Defense
 *  Extreme Scale Systems Center ( ESSC ) http://www.csm.ornl.gov/essc/
 *  and the wonderful people at I/O Performance, Inc.
 */
/*
 * This file contains the subroutines necessary to manage everything to
 * do with restarting an xddcp operation.
 */
#include "xdd.h"

// Prototypes
int xdd_restart_create_restart_file(restart_t *rp);
int xdd_restart_read_restart_file(restart_t *rp);
int xdd_restart_write_restart_file(restart_t *rp);

/*----------------------------------------------------------------------------*/
// This routine is called to create a new restart file when a new copy 
// operation is started for the first time.
int 
xdd_restart_create_restart_file(restart_t *rp) {

	char	*homep;			// Pointer to a string that contains the home directory
	time_t	t;				// Time structure
	struct	tm		*tm;			// Broken-down time
	char	*filename;
	

fprintf(stderr,"RESTART_MONITOR: CREATE_RESTART_FILE enter...rp is %x\n",rp);
	// Get the current home directory name
	homep = getenv("HOME");
	if (homep == NULL) {
		fprintf(xgp->errout,"%s: WARNING: No home directory specified, using current working directory for restart file\n",xgp->progname);
		perror("Reason");
		homep = ".";
	}
	filename = malloc(MAX_TARGET_NAME_LENGTH);
	if (filename == NULL) {
		fprintf(xgp->errout,"%s: ALERT: Cannot allocate %d bytes of memory for the restart file name\n",
			xgp->progname,
			MAX_TARGET_NAME_LENGTH);
		perror("Reason");
		rp->fp = xgp->errout;
		return(0);
	}

	// Get the current time in a appropriate format for a file name
	time(&t);
	tm = localtime(&t);
	sprintf(filename,"%s/%s.%s.%s.%s.%s.%02d.%02d.%2d.%02d.%02d.%02d.rst",
		homep,
		(rp->flags & RESTART_FLAG_ISSOURCE)?"source":"destination",
		(rp->source_host==NULL)?"NA":rp->source_host,
		(rp->source_filename==NULL)?"NA":basename(rp->source_filename),
		(rp->destination_host==NULL)?"NA":rp->destination_host,
		(rp->destination_filename==NULL)?"NA":basename(rp->destination_filename),
		tm->tm_mon+1,  // month since January - range 0 to 11 
		tm->tm_mday,   // day of the month range 1 to 31
		tm->tm_year+1900, // number of years since 1900
		tm->tm_hour,
		tm->tm_min,
		tm->tm_sec);

	fprintf(stderr,"RESTART_MONITOR: CREATE_RESTART_FILE name is %s\n",filename);

	rp->fp = fopen(filename,"w");
	if (rp->fp == NULL) {
		fprintf(xgp->errout,"%s: ERROR: Cannot create restart file %s!\n",
			xgp->progname,
			filename);
		perror("Reason");
		rp->fp = xgp->errout;
		return(-1);
	}
	
	// Success - everything must have worked and we have a restart file
	fprintf(xgp->errout,"%s: RESTART: Successfully created restart file %s!\n",
		xgp->progname,
		filename);
	rp->restart_filename = filename; // Save this file name in this restart structure
	return(0);
}
/*----------------------------------------------------------------------------*/
// This routine is called to read an existing restart file when trying to
// restart a copy operation
// 
int
xdd_restart_read_restart_file(restart_t *rp) {

}
/*----------------------------------------------------------------------------*/
// This routine is called to write new information to an existing restart file 
// during a copy operation - this is also referred to as a "checkpoint"
// operation. Each time the write is performed to this file it is flushed to
// the storage device. 
// 
int
xdd_restart_write_restart_file(restart_t *rp) {
	int		status;
	char	output[256];

	// Seek to the beginning of the file 
	status = fseek(rp->fp, 0L, SEEK_SET);
	if (status < 0) {
		fprintf(xgp->errout,"%s: RESTART: WARNING: Seek to beginning of restart file %s failed\n",
			xgp->progname,
			rp->restart_filename);
		perror("Reason");
	}
	
	// Issue a write operation for the stuff
	fprintf(rp->fp,"-restart offset %lld\n", rp->last_committed_location);

	// Flush the file for safe keeping
	fflush(rp->fp);

	return(SUCCESS);
}
/*----------------------------------------------------------------------------*/
// This routine is created when xdd starts a copy operation (aka xddcp).
// This routine will run in the background and waits for various xdd I/O
// qthreads to update their respective counters and enter the barrier to 
// notify this routine that a block has been read/written.
// This monitor runs on both the source and target machines during a copy
// operation as is indicated in the name of the restart file. The information
// contained in the restart file has different meanings depending on which side
// of the copy operation the restart file represents. 
// On the source side, the restart file contains information regarding the lowest 
// and highest byte offsets into the source file that have been successfully 
// "read" and "sent" to the destination machine. This does not mean that the 
// destination machine has actually received the data but it is an indicator of 
// what the source side thinks has happened.
// On the destination side the restart file contains information regarding the
// lowest and highest byte offsets and lengths that have been received and 
// written (committed) to stable media. This is the file that should be used
// for an actual copy restart opertation as it contains the most accurate state
// of the destination file.
//
// This routine will also display information about the copy operation before,
// during, and after xddcp is complete. 
// 
void *
xdd_restart_monitor(void *junk) {
	int		target_number;
	int		qthread_number;
	ptds_t	*current_ptds;
	ptds_t	*current_qthread;
	int32_t low_qthread_number;
	uint64_t low_byte_offset;
	uint64_t high_offset;
	uint64_t offset_separation;
	uint64_t check_counter;
	int		status;

	

	// Initialize stuff
fprintf(stderr,"RESTART_MONITOR: Initializing...\n");
	for (target_number=0; target_number < xgp->number_of_targets; target_number++) {
		current_ptds = xgp->ptdsp[target_number];
		status = xdd_restart_create_restart_file(current_ptds->restartp);
	}
	// Go into a loop that periodically checks all the targets/qthreads 

	check_counter = 0;
	for (;;) {
		// Sleep for the specified period of time
		sleep(xgp->restart_frequency);

		check_counter++;
		// Check all targets
		for (target_number=0; target_number < xgp->number_of_targets; target_number++) {
			current_ptds = xgp->ptdsp[target_number];
			// If this target does not require restart monitoring then continue
			if ( !(current_ptds->target_options & TO_RESTART_ENABLE) ) // if restart is NOT enabled for this target then continue
				continue;
			current_qthread = current_ptds;
			// Check all qthreads on this target
			low_byte_offset = current_qthread->restartp->last_committed_location;
			low_qthread_number = current_qthread->my_qthread_number;
			while (current_qthread) {
				if ((current_qthread->my_current_state & CURRENT_STATE_COMPLETE) && // if this target has completed all I/O AND the restart monitor has checked it the continue
					(current_qthread->my_current_state & CURRENT_STATE_RESTART_COMPLETE)) {
					current_qthread = current_qthread->nextp;
					continue;
				}

				// At this point this qthread needs to be checked. If it had finished but had not been checked then set the flag saying it has now been checked
				if (current_qthread->my_current_state & CURRENT_STATE_COMPLETE) // If this target has finished then mark the Restart Complete as well
					current_qthread->my_current_state |= CURRENT_STATE_RESTART_COMPLETE;
	
				// If information has not changed since last time, just increment the montior count and continue
                   //////tbd //////
				// If information has changed then set the monitor count to 0, save the byte offset, and continue
				if ( current_qthread->restartp->last_committed_location < low_byte_offset) {
					low_byte_offset = current_qthread->restartp->last_committed_location; // the new low_byte_offset
					low_qthread_number = current_qthread->my_qthread_number;
				}
				current_qthread = current_qthread->nextp;
			} // End of WHILE loop that looks at all qthreads for this target
			
			// Now that we have all the information for this target's qthreads, generate the appropriate information
			// and write it to the restart file and sync sync sync
			current_ptds->restartp->low_byte_offset = low_byte_offset;
			xdd_restart_write_restart_file(current_ptds->restartp);

		} // End of FOR loop that checks all targets 
		// Done checking all targets

		// If it is time to leave then leave - the qthread cleanup will take care of closing the restart files
			if (xgp->abort_io | xgp->restart_terminate) 
				break;
	} // End of FOREVER loop that checks stuff
	fprintf(xgp->output,"%s: RESTART Monitor is exiting\n",xgp->progname);
	return;
}