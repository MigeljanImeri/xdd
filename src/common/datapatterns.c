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
 * This file contains the subroutines that initialize various data patterns
 * used by XDD
 */
#define XDD_DATA_PATTERN
#include <assert.h>
#include "xint.h"
#include <sys/sysinfo.h>

#define WHOLEFILE_MAX_SIZE_RAM 0.5 // Currently only allow for 50% of RAM

int xdd_data_pattern_init(xint_data_pattern_t* xdp)
{
    assert(0 != xdp);
    memset(xdp, 0, sizeof(*xdp));
    return 0;
}

/*----------------------------------------------------------------------------*/
/* xdd_datapattern_buffer_init() - init the I/O buffer with the appropriate pattern
 * This routine will put the requested pattern in the rw buffer.
 */
void
xdd_datapattern_buffer_init(worker_data_t *wdp) {
	target_data_t	*tdp;
    int32_t i;
    int32_t pattern_length; // Length of the pattern
    size_t remaining_length; // Length of the space in the pattern buffer
    unsigned char    *ucp;          // Pointer to an unsigned char type, duhhhh
    uint32_t *lp;			// pointer to a pattern
    xint_data_pattern_t	*dpp;


	tdp = wdp->wd_tdp;
    dpp = tdp->td_dpp;
	if (dpp->data_pattern_options & DP_WHOLEFILE_PATTERN) { // Using the whole contents of the file
		if (tdp->td_dpp->data_pattern_length < ((size_t)tdp->td_xfer_size)) {
			memset(wdp->wd_task.task_datap,'\0',tdp->td_xfer_size);
			ucp = (unsigned char *)wdp->wd_task.task_datap;
			if (dpp->data_pattern_options & DP_REPLICATE_PATTERN) { // Replicate the pattern throughout the buffer
				remaining_length = tdp->td_xfer_size;
				while (remaining_length) {
					if (dpp->data_pattern_length < remaining_length)
						pattern_length = dpp->data_pattern_length;
					else pattern_length = remaining_length;
					memcpy(ucp,dpp->data_pattern,pattern_length);
					remaining_length -= pattern_length;
					ucp += pattern_length;
				}
			}
		} else if (tdp->td_dpp->data_pattern_length % tdp->td_xfer_size) {
			/* 
			 * We will be assign data using each worker_data_t's task_byte_offset during target passes,
			 * so we just need to check if transfer size is evenly divisable by the
			 * the target file size here.
			 *
			 * If the Transfer size is not evenly divisable, we will warn users as this will
			 * require periodic memcpy's to occur during target passes and effect measurements.
			 */
			fprintf(xgp->errout, "%s: WARNING file %s's size %lu is not evenly divisable by target transfer size %u this will effect performance measurements\n",
				xgp->progname, 
				tdp->td_dpp->data_pattern_filename,
				tdp->td_dpp->data_pattern_length,
				tdp->td_xfer_size);
			// Since the whole file size is greater than the td_xfer_size we will remove
			// the replicate flag.
			tdp->td_dpp->data_pattern_options = 
				tdp->td_dpp->data_pattern_options & ~DP_REPLICATE_PATTERN;
		}
	} else if (dpp->data_pattern_options & DP_RANDOM_PATTERN) { // A nice random pattern
		lp = (uint32_t *)wdp->wd_task.task_datap;
		xgp->random_initialized = 0;
		xgp->random_init_seed = 72058; // Backward compatibility with older xdd versions
		/* Set each four-byte field in the I/O buffer to a random integer */
		for(i = 0; i < (int32_t)(tdp->td_xfer_size / sizeof(int32_t)); i++ ) {
	    	*lp=xdd_random_int();
	    	lp++;
		}
    } else if (dpp->data_pattern_options & DP_RANDOM_BY_TARGET_PATTERN) { // A nice random pattern, unique by target number
		lp = (uint32_t *)wdp->wd_task.task_datap;
		xgp->random_initialized = 0;
		xgp->random_init_seed = (tdp->td_target_number+1); 
		/* Set each four-byte field in the I/O buffer to a random integer */
		for(i = 0; i < (int32_t)(tdp->td_xfer_size / sizeof(int32_t)); i++ ) {
	    	*lp=xdd_random_int();
	    	lp++;
		}
    } else if ((dpp->data_pattern_options & DP_ASCII_PATTERN) ||
	     (dpp->data_pattern_options & DP_HEX_PATTERN)) { // put the pattern that is in the pattern buffer into the io buffer
		// Clear out the buffer before putting in the string so there are no strange characters in it.
		memset(wdp->wd_task.task_datap,'\0',tdp->td_xfer_size);
		if (dpp->data_pattern_options & DP_REPLICATE_PATTERN) { // Replicate the pattern throughout the buffer
	    	ucp = (unsigned char *)wdp->wd_task.task_datap;
	    	remaining_length = tdp->td_xfer_size;
	    	while (remaining_length) { 
				if (dpp->data_pattern_length < remaining_length) 
		    		pattern_length = dpp->data_pattern_length;
				else pattern_length = remaining_length;
		
				memcpy(ucp,dpp->data_pattern,pattern_length);
				remaining_length -= pattern_length;
				ucp += pattern_length;
	    	}
		} else { // Just put the pattern at the beginning of the buffer once 
	    	if (dpp->data_pattern_length < (size_t)tdp->td_xfer_size) 
				pattern_length = dpp->data_pattern_length;
	    	else pattern_length = tdp->td_xfer_size;
	    	memcpy(wdp->wd_task.task_datap,dpp->data_pattern,pattern_length);
		}
    } else if (dpp->data_pattern_options & DP_LFPAT_PATTERN) {
		memset(wdp->wd_task.task_datap,0x00,tdp->td_xfer_size);
		dpp->data_pattern_length = sizeof(lfpat);
		fprintf(stderr,"LFPAT length is %d\n", (int)dpp->data_pattern_length);
		memset(wdp->wd_task.task_datap,0x00,tdp->td_xfer_size);
		remaining_length = tdp->td_xfer_size;
		ucp = (unsigned char *)wdp->wd_task.task_datap;
		while (remaining_length) { 
	    	if (dpp->data_pattern_length < remaining_length) 
				pattern_length = dpp->data_pattern_length;
	    	else pattern_length = remaining_length;
	    	memcpy(ucp,lfpat,pattern_length);
	    	remaining_length -= pattern_length;
	    	ucp += pattern_length;
		}
    } else if (dpp->data_pattern_options & DP_LTPAT_PATTERN) {
		memset(wdp->wd_task.task_datap,0x00,tdp->td_xfer_size);
		dpp->data_pattern_length = sizeof(ltpat);
		fprintf(stderr,"LTPAT length is %d\n", (int)dpp->data_pattern_length);
		memset(wdp->wd_task.task_datap,0x00,tdp->td_xfer_size);
		remaining_length = tdp->td_xfer_size;
		ucp = (unsigned char *)wdp->wd_task.task_datap;
		while (remaining_length) { 
	    	if (dpp->data_pattern_length < remaining_length) 
				pattern_length = dpp->data_pattern_length;
	    	else pattern_length = remaining_length;
	    	memcpy(ucp,ltpat,pattern_length);
	    	remaining_length -= pattern_length;
	    	ucp += pattern_length;
		}
    } else if (dpp->data_pattern_options & DP_CJTPAT_PATTERN) {
		memset(wdp->wd_task.task_datap,0x00,tdp->td_xfer_size);
		dpp->data_pattern_length = sizeof(cjtpat);
		fprintf(stderr,"CJTPAT length is %d\n", (int)dpp->data_pattern_length);
		memset(wdp->wd_task.task_datap,0x00,tdp->td_xfer_size);
		remaining_length = tdp->td_xfer_size;
		ucp = (unsigned char *)wdp->wd_task.task_datap;
		while (remaining_length) { 
	    	if (dpp->data_pattern_length < remaining_length) 
				pattern_length = dpp->data_pattern_length;
	    	else pattern_length = remaining_length;
	    	memcpy(ucp,cjtpat,pattern_length);
	    	remaining_length -= pattern_length;
	    	ucp += pattern_length;
		}
    } else if (dpp->data_pattern_options & DP_CRPAT_PATTERN) {
		memset(wdp->wd_task.task_datap,0x00,tdp->td_xfer_size);
		dpp->data_pattern_length = sizeof(crpat);
		fprintf(stderr,"CRPAT length is %d\n", (int)dpp->data_pattern_length);
		memset(wdp->wd_task.task_datap,0x00,tdp->td_xfer_size);
		remaining_length = tdp->td_xfer_size;
		ucp = (unsigned char *)wdp->wd_task.task_datap;
		while (remaining_length) { 
	    	if (dpp->data_pattern_length < remaining_length) 
				pattern_length = dpp->data_pattern_length;
	    	else pattern_length = remaining_length;
	    	memcpy(ucp,crpat,pattern_length);
	    	remaining_length -= pattern_length;
	    	ucp += pattern_length;
		}
    } else if (dpp->data_pattern_options & DP_CSPAT_PATTERN) {
		memset(wdp->wd_task.task_datap,0x00,tdp->td_xfer_size);
		dpp->data_pattern_length = sizeof(cspat);
		fprintf(stderr,"CSPAT length is %d\n", (int)dpp->data_pattern_length);
		memset(wdp->wd_task.task_datap,0x00,tdp->td_xfer_size);
		remaining_length = tdp->td_xfer_size;
		ucp = (unsigned char *)wdp->wd_task.task_datap;
		while (remaining_length) { 
	    	if (dpp->data_pattern_length < remaining_length) 
				pattern_length = dpp->data_pattern_length;
	    	else pattern_length = remaining_length;
	    	memcpy(ucp,cspat,pattern_length);
	    	remaining_length -= pattern_length;
	    	ucp += pattern_length;
		}
    } else if (dpp->data_pattern_options & DP_FILE_PATTERN) {
		memset(wdp->wd_task.task_datap,'\0',tdp->td_xfer_size);
		ucp = (unsigned char *)wdp->wd_task.task_datap;
		if (dpp->data_pattern_options & DP_REPLICATE_PATTERN) { // Replicate the pattern throughout the buffer
			remaining_length = tdp->td_xfer_size;
			while (remaining_length) {
				if (dpp->data_pattern_length < remaining_length)
					pattern_length = dpp->data_pattern_length;
				else pattern_length = remaining_length;
				memcpy(ucp,dpp->data_pattern,pattern_length);
				remaining_length -= pattern_length;
				ucp += pattern_length;
			}
		} else { // Just put the pattern at the beginning of the buffer once
			if (dpp->data_pattern_length < (size_t)tdp->td_xfer_size)
				pattern_length = dpp->data_pattern_length;
			else pattern_length = tdp->td_xfer_size;
			memcpy(ucp,dpp->data_pattern,pattern_length);
		}
	} else { // Otherwise set the entire buffer to the character in "dpp->data_pattern"
		memset(wdp->wd_task.task_datap,*(dpp->data_pattern),tdp->td_xfer_size);
   	}
		
} // end of xdd_datapattern_buffer_init()

/*----------------------------------------------------------------------------*/
/* xdd_datapattern_fill() - This subroutine will fill the buffer with a 
 * specific pattern. 
 * This routine is called within the inner I/O loop for every I/O if the data
 * pattern changes from IO to IO
 */
void
xdd_datapattern_fill(worker_data_t *wdp) {
	target_data_t	*tdp;
	size_t  		j;					// random variables 
	uint64_t 		*posp;             	// Position Pointer 
	nclk_t			start_time;			// Used for calculating elapsed times of ops
	nclk_t			end_time;			// Used for calculating elapsed times of ops


	tdp = wdp->wd_tdp;
	/* Sequenced Data Pattern */
	if (tdp->td_dpp->data_pattern_options & DP_SEQUENCED_PATTERN) {
		nclk_now(&start_time);
		posp = (uint64_t *)wdp->wd_task.task_datap;
		for (j=0; j<(tdp->td_xfer_size/sizeof(wdp->wd_task.task_byte_offset)); j++) {
			*posp = wdp->wd_task.task_byte_offset + (j * sizeof(wdp->wd_task.task_byte_offset));
			*posp |= tdp->td_dpp->data_pattern_prefix_binary;
			if (tdp->td_dpp->data_pattern_options & DP_INVERSE_PATTERN)
				*posp ^= 0xffffffffffffffffLL; // 1's compliment of the pattern
			posp++;
		}
		nclk_now(&end_time);
// FIXME ????		wdp->wd_accumulated_pattern_fill_time = (end_time - start_time);
	}
} // End of xdd_datapattern_fill() 

/*----------------------------------------------------------------------------*/
/* xdd_datapattern_wholefile_enough_ram() - This subroutine will verify that
 * if a the flag wholefile is used that the size of the file is no more than
 * WHOLEFILE_MAX_SIZE_RAM.
 */
int
xdd_datapattern_wholefile_enough_ram(target_data_t *tdp, const char *filename) {
	struct sysinfo info;
	size_t file_size = tdp->td_dpp->data_pattern_length;

	assert(tdp->td_dpp->data_pattern_options & DP_WHOLEFILE_PATTERN);

	// Short circuit empty files
	if (!file_size)
		goto error;

	sysinfo(&info);
	if (file_size > (info.freeram * WHOLEFILE_MAX_SIZE_RAM))
		goto error;;

	return TRUE;

error:
	fprintf(xgp->errout, "%s: file %s can not be loaded into memory because 50%% of RAM is "
		"currently unavailable\n",
		xgp->progname,
		filename);

	return FALSE;
} // End of xdd_datapattern_wholefile_enough_ram() 

/*----------------------------------------------------------------------------*/
/* xdd_set_datapattern_from_filename() - This subroutine will set the targets
 * data_pattern from a file. The targets td_dpp->data_pattern_length must be
 * set before calling this. On error returns 1, and on success returns 0.
 */
int
xdd_set_datapattern_from_filename(target_data_t *tdp, char *filename) {
	struct xint_data_pattern *dp = tdp->td_dpp;
	int ret = 0;

	int fd = open(filename, O_RDONLY);
	if (fd < 0 ) {
		fprintf(xgp->errout, "%s, could not open %s\n", xgp->progname, filename);
		return  1;
	}

	dp->data_pattern_filename = filename;
	ret = xdd_set_datapattern_from_file_descriptor(tdp, fd, filename);
	close(fd);

	return ret;
} // End of xdd_set_datapattern_from_filename() 

/*----------------------------------------------------------------------------*/
/* xdd_set_datapattern_from_file_descriptor() - This subroutine will set the
 * targets data_pattern from an open file descriptor. The targets
 * td_dpp->data_pattern_length must be set before calling this. On error returns 1,
 * and on success returns 0.
 */
int
xdd_set_datapattern_from_file_descriptor(target_data_t *tdp, int fd, char *filename) {
	struct xint_data_pattern *dp = tdp->td_dpp;

	// Short circuit if the file is not open
	if (fd < 0) {
		return 1;
	}

	dp->data_pattern_filename = filename;
	dp->data_pattern = (unsigned char *)malloc(sizeof(unsigned char) * dp->data_pattern_length + 1)	;
	if (!dp->data_pattern) {
		fprintf(xgp->errout, "%s: could not allocate datapattern buffer for target %d",
			xgp->progname, tdp->td_target_number);
		goto error;
	}

	size_t bytes = pread(fd, dp->data_pattern, dp->data_pattern_length, 0);
	if (bytes != dp->data_pattern_length) {
		fprintf(xgp->errout, "%s: short read from file %s when setting target datapattern. "
			"Espected %zu bytes but got %zu",
			xgp->progname, filename, dp->data_pattern_length, bytes);
			goto error;
	}

	return 0;

error:
	free(dp->data_pattern);
	return 1;
} // End of xdd_set_datapattern_from_file_descriptor() 

/*----------------------------------------------------------------------------*/
/* xdd_datapattern_get_datap_from_offset() - This subroutine will return the
 * beginning of a the buffer from the target's tp_dpp->data_pattern based on
 * the offset in the buffer.
 */
unsigned char *
xdd_datapattern_get_datap_from_offset(worker_data_t *wdp) {
	target_data_t *tdp;
	unsigned char *buff = NULL;
	off_t offset = 0;
	size_t memcpy_size = 0;

	tdp = wdp->wd_tdp;
	
	if (tdp->td_dpp->data_pattern_options & DP_WHOLEFILE_PATTERN) {
		/*
		 * In the event of a whole file data pattern we can wind up with offsets that
		 * are not correctly aligned with the target data buffer size. See Below:
		 * 
		 * tdp->td_dpp->data_pattern - TDP
		 * wdp->wd_task.task_datap - WDP
		 * 
		 * Aligned:
		 *                              TDP
		 * |-----------------------------------------------------------|
		 * | ------------ | ------------ | ------------ | ------------ | 
		 *	     WDP1           WDP2           WDP3           WDP4
		 *
		 * Unaligned:
		 *                              TDP
		 * |-----------------------------------------------------------|
		 * | ---------- | ---------- | ---------- | ---------- | ---------- | 
		 *	    WDP1		 WDP2         WDP3		   WDP4			WDP5
		 *
		 * In the aligned case, we can just simply modulo the work_data_t's byte offset
		 * by the size of TDP. However, in the unaligned case we have to grab the trailing
		 * data and the add the beginning data from the TDP into WP5. See Below:
		 *
		 *                              TDP
		 * |-----------------------------------------------------------|
		 * | ---- |										       |-------|
		 *	 WDP5												  WDP5
		 *
		 * In the unaligned case we will allocate the IO buffer for the worker and copy
		 * the correct data to it.
		 */  
		offset = wdp->wd_task.task_byte_offset % tdp->td_dpp->data_pattern_length;
		if ((tdp->td_dpp->data_pattern_length - offset) < (size_t)tdp->td_xfer_size) { 
			// The target's xfer_size does not evenly divide into the whole file's
			// size, so we have to allocate a buffer (if not already allocated) to
			// hold the data an copy it over.
			if (!wdp->wd_bufp_allocated) {
				buff = xdd_init_io_buffers(wdp);
				wdp->wd_task.task_datap = buff;
			} else {
				buff = wdp->wd_task.task_datap;
			}
			memcpy_size = tdp->td_dpp->data_pattern_length - offset; 
			memcpy(buff, &(tdp->td_dpp->data_pattern[offset]), memcpy_size);
			memcpy(&buff[memcpy_size], tdp->td_dpp->data_pattern, (tdp->td_xfer_size - memcpy_size));
		} else {
			buff = &(tdp->td_dpp->data_pattern[offset]);
		}
	} else {
		// We are not using a whole file for the data pattern so just return
		// the worker thread's buffer.
		buff = wdp->wd_task.task_datap;
	}

	return buff;
} // End of xdd_datapattern_get_datap_from_offset() 
 
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
