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
 *       Brad Settlemyer, DoE/ORNL
 *       Russell Cattelan, Digital Elves
 *       Alex Elder
 * Funding and resources provided by:
 * Oak Ridge National Labs, Department of Energy and Department of Defense
 *  Extreme Scale Systems Center ( ESSC ) http://www.csm.ornl.gov/essc/
 *  and the wonderful people at I/O Performance, Inc.
 */
/*
 * This file contains the table of subroutines necessary to parse the command line
 * arguments  set up all the global and target-specific variables.
 */

#include "xdd.h"
/* -------------------------------------------------------------------------------------------------- */
// The xdd Function Table
//   This table contains entries of the following structure as defined in xdd.h:
//            struct xdd_func {
//	              char    *func_name;     /* name of the function */
//	              char    *func_alt;      /* Alternate name of the function */
//                int     (*func_ptr)(int32_t argc, char *argv[], uint32_t flags, uint32_t flags);      /* pointer to the function */
//                int     argc;           /* number of arguments */
//                char    *help;          /* help string */
//                char    *ext_help[5];   /* Extented help strings */
//            };
xdd_func_t  xdd_func[] = {
    {"blocksize", "bs",
            xddfunc_blocksize,  
            1,  
            "  -blocksize [target <target#>] <#bytes/block>\n",  
            {"    Specifies the size of a single 'block'.\n", 
            0,0,0,0},
			0},
    {"bytes",  "b",
            xddfunc_bytes,     
            1,  
            "  -bytes [target <target#>] <#>\n",  
            {"    Specifies the number of bytes to transfer during a single pass\n", 
            0,0,0,0},
			0},
    {"combinedout", "combo",
            xddfunc_combinedout,
            1,  
            "  -combinedout <filename>\n",  
            {"    Will direct a copy of just the Combined output lines to be 'appended' to the specified file\n", 
            0,0,0,0},
			0},
    {"createnewfiles",  "cnf",
            xddfunc_createnewfiles,  
            1,  
            "  -createnewfiles [target <target#>]\n",  
            {"    Will continually create new target files by appending a number to the target file name for each new file\n", 
            0,0,0,0},
			0},
    {"csvout", "csvo",
            xddfunc_csvout,     
            1,  
            "  -csvout <filename>\n",  
            {"    Will direct Comma Separated Values output to the specified file\n", 
            0,0,0,0},
			0},
    {"datapattern", "dp",
            xddfunc_datapattern,    
            1,  
            "  -datapattern [target <target#>] <c> | random | sequenced | prefix <hexdigits> | inverse | ascii <asciistring> | hex <hexdigits> | replicate | lfpat | ltpat | cjtpat | crpat | cspat\n",  
            {"    -datapattern 'c' will use the character c as the data pattern to write\n\
       If the word 'random' is specified for the pattern then a random pattern will be generated\n\
       If the word 'sequenced' is specified for the pattern then a sequenced number pattern will be generated\n\
       If the word 'prefix' is specified for the pattern then the specified hex digits will be placed in the upper N bits of the 64-bit pattern\n\
       If the word 'inverse' is specified for the pattern then the actual pattern will be the 1's compliment of the specified pattern\n\
       If the word 'ascii' is specified then the following string is used as a single pattern\n\
       If the word 'hex' is specified then the following hex characters <0-9,a-f or A-F> are used as the pattern\n\
       If the word 'replicate' is specified then whatever pattern was specified is replicated throughout the buffer\n",
      "If any of the words 'lfpat, ltpat, cjtpat, crpat, or cspat' is specified then the 8B/10B stress patterns are used.\n\
	        Default data pattern is all binary 0's\n",
             0,0},
			0},
    {"debug", "debug",
            xddfunc_debug,
            1,
            "  -debug\n",
            {"    Will print out debug information, more for each instance\n",
            0,0,0,0},
			XDD_FUNC_INVISIBLE},
    {"deletefile",  "del",
            xddfunc_deletefile,  
            1,  
            "  -deletefile [target <target#>]\n",  
            {"    Will delete the target file upon completion IFF the target is a regular file\n", 
            0,0,0,0},
			0},
    {"deskew", "deskew",
            xddfunc_deskew,     
            1,  
            "  -deskew\n",  
            {"    Will cause the calculated bandwidth and IOPs to be deskewed before being reported\n", 
            0,0,0,0},
			0},
    {"devicefile",  "devf",
            xddfunc_devicefile, 
            1,  
            "  -devicefile\n",  
            {"    Indicates that the file being accessed is a device - Windows only", 
            0,0,0,0},
			XDD_FUNC_INVISIBLE},
    {"directio",  "dio",
            xddfunc_dio,        
            1,  
            "  -dio [target <target#>]\n",  
            {"    Will use DIRECTIO on targets that are files\n", 
            0,0,0,0},
			0},
    {"dryrun",  "dry",
            xddfunc_dryrun,        
            1,  
            "  -dryrun \n",  
            {"    Will use exit after parsing the arguments and displaying initialization information.\n", 
            0,0,0,0},
			0},
    {"endtoend", "e2e",
            xddfunc_endtoend,
            1,
            "  -endtoend [target #]  issource | isdestination | destination <hostname> | port <#> \n",
            {"    Specifies a source and destination information for doing end-to-end test between two machines",
            0,0,0,0},
			0},
    {"errout", "eo",
            xddfunc_errout,     
            1,  
            "  -errout <filename>\n",  
            {"    Will direct all error output to the specified file\n", 
            0,0,0,0},
			0},
    {"extendedstats", "extstats",
            xddfunc_extended_stats,     
            1,  
            "  -extendedstats\n",  
            {"    Will calculate extended op stats\n", 
            0,0,0,0},
			XDD_FUNC_INVISIBLE},
    {"flushwrite", "fw",
            xddfunc_flushwrite,     
            1,  
            "  -flushwrite [target <target#>] <#>\n",  
            {"    Perform a flush operation every so many write operations\n", 
            0,0,0,0},
			0},
    {"fullhelp", "exthelp",
            xddfunc_fullhelp,   
            1,  
            "  -fullhelp\n",  
            {"    Extended Help - displays all kinds of useful information", 
            0,0,0,0},
			0},
    {"heartbeat", "hb",
            xddfunc_heartbeat,  
            1,  
            "  -heartbeat # | ops | bytes | kbytes | mbytes | gbytes | percent | bw | iops | et | ignorerestart\n",  
            {"    Will print out heartbeat information every # seconds \n\
 			     'operations' | 'ops' - current number of operations complete \n\
                 'bytes' | 'b' -  current bytes transfered \n\
                 'kbytes' | 'kb' - current Kilo Bytes transfered \n\
                 'mbytes' | 'mb' - current Mega Bytes transfered \n\
                 'gbytes' | 'gb' - current Giga Bytes transfered \n\
                 'percent' | 'pct' - percent complete\n\
                 'bandwidth' | 'bw' -  aggregate bandiwdth\n\
                 'iops' - aggregate I/O operations per second\n\
                 'et' | 'etc' | 'eta' - estimated time to completion\n\
                 'ignorerestart' | 'ir' - ignore the fact that a restart is in process\n",
			 "Specifying -heartbeat multiple times will add these to the heartbeat output string FOR EACH TARGET\n",
            0,0,0},
			0},
    {"help", "h",
            xddfunc_help,  
            1,  
            "  -help <option_name>\n",  
            {"    Will print out extended help for a specific option \n", 
            0,0,0,0},
			0},
    {"identifier",   "id",
            xddfunc_id, 
            1,  
            "  -id \"string\" | commandline \n",  
            {"    Specifies an arbitrary ID string to place in the time stamp file and printed on stdout\n", 
            0,0,0,0},
			0},
    {"interactive",   "inter",
            xddfunc_interactive, 
            1,  
            "  -interactive\n",  
            {"    Indicates that XDD should start up in Interactive Mode - targets will not start until the 'run' command is given.\n", 
            0,0,0,0},
			0},
    {"kbytes",  "kb",
            xddfunc_kbytes,     
            1,  
            "  -kbytes [target <target#>] <#>\n",  
            {"    Specifies the number of 1024-byte blocks to transfer during a single pass\n", 
            0,0,0,0},
			0},
    {"lockstep", "ls",
            xddfunc_lockstep,   
            1,  
            "  -lockstep <mastertarget#> <slavetarget#> <time|op|percent|mbytes|kbytes> # <time|op|percent|mbytes|kbytes># <wait|run> <complete|stop>\n",   
            {"  Where 'master_target' is the target that tells the slave when to do something.\n\
        'slave_target' is the target that responds to requests from the master.\n\
        'when' specifies when the master should tell the slave to do something.\n\
  The word 'when' should be replaced with the word: \n\
        'time', 'op', 'percent', 'mbytes', 'kbytes'.\n\
        'howlong' is either the number of seconds, number of operations, ...etc.\n\
        - The interval time in seconds <a floating point number> between task requests from the\n\
          master to the slave. i.e. if this number were 2.3 then the master would request\n\
          the slave to perform its task every 2.3 seconds until.\n\
        - The operation number that defines the interval on which the master will request\n\
          the slave to perform its task. i.e. if the operation number is set to 8 then upon\n\
          completion of every 8 <master> operations, the master will request the slave to perform its task.\n\
        - The percentage of operations that must be completed by the master before requesting\n\
          the slave to perform a task.\n\
        - The number of megabytes <1024*1024 bytes> or the number of kilobytes <1024 bytes>\n\
        'what' is the type of task the slave should perform each time it is requested to perform\n\
          a task by the master. The word 'what' should be replaced by:\n\
        'time', 'op', 'percent', 'mbytes', 'kbytes'.\n",
             "    'howmuch' is either the number of seconds, number of operations, ...etc.\n\
	    - The amount of time in seconds <a floating point number> the slave should run before\n\
          pausing and waiting for further requests from the master.\n\
        - The number of operations the slave should perform before pausing and waiting for\n\
          further requests from the master.\n\
        - The number of megabytes <1024*1024 bytes> or the number of kilobytes <1024 bytes>\n\
          the slave should transfer before pausing and waiting for further requests from the master.\n\
        'startup' is either 'wait' or 'run' depending on whether the slave should start running upon\n\
          invocation and perform a single task or if it should simply wait for the master to \n\
          request it to perform its first task. \n\
        'Completion' - in the event that the master finishes before the slave, then the slave will have\n\
          the option to complete all of its remaining operations or to just stop at this point.\n\
          This should be specified as either 'complete' or 'stop'.\n",
                         0,0,0},
			0},
    {"looseordering", "lo",
            xddfunc_looseordering,     
            1,  
            "  -looseordering [target <target#>]\n",  
            {"    Will enforce for Loose Ordering of QThreads for all targets or a specified target. Default is No Ordering\n", 
             "    see also: -strictordering or -noordering\n",
			0,0,0},
			0},
    {"maxall", "maxall",
            xddfunc_maxall,     
            1,  
            "  -maxall\n",  
            {"    Will set maximumpriority and processlock\n", 
            0,0,0,0},
			0},
    {"maxerrors", "maxerrors",
            xddfunc_maxerrors,  
            1,  
            "  -maxerrors #\n",  
            {"    Specifies the total number of errors to tolerate before giving up\n",
             "    The default is to NEVER give up\n",
             0,0,0},
			0},
    {"maxerrorstoprint", "mep",
            xddfunc_max_errors_to_print,  
            1,  
            "  -maxerrorstoprint #\n",  
            {"    Specifies the total number of errors to print information about - this mainly applies to 'compare' errors for -verify contents\n",
             "    The default is about 10\n",
             0,0,0},
			0},
    {"maximumpriority", "maxpri",     
            xddfunc_maxpri,     
            1,  
            "  -maxpri\n",  
            {"    Will set the process to maximum priority\n", 
            0,0,0,0},
			0},
    {"mbytes", "mb",
            xddfunc_mbytes,     
            1,  
            "  -mbytes [target <target#>] <#>\n",  
            {"    Specifies the number of 1024*1024-byte blocks to transfer in a single pass\n", 
            0,0,0,0},
			0},
    {"memalign", "mal",
            xddfunc_memalign,      
            1,  
            "  -memalign [target <target#>] <#bytes>\n",  
            {"    Align memory on an #-byte boundary - should be an even number\n", 
            0,0,0,0},
			0},
    {"minall", "minall",
            xddfunc_minall,     
            1,  
            "  -minall\n",  
            {"    Will set not lock memory, process, or reset process priority\n", 
            0,0,0,0},
			0},
    {"multipath", "mp",
            xddfunc_multipath,     
            1,  
			"  -multipath paths <#paths> <target#> <target#> <target#> ... | roundrobin | rr | load <percent_load> | useonly <#paths> <target#> <target#> ...\n",  
            {"    Will set up an alternate path access for the specified targets. Path switching is based on one of the stated policies\n\
      which are 'roundrobin' <aka 'rr'> or 'load' or 'useonly'\n", 
            0,0,0,0},
			XDD_FUNC_INVISIBLE},
    {"nobarrier", "nb",
            xddfunc_nobarrier,  
            1,  
            "  -nobarrier\n",  
            {"    Turns off barriers before operation. All threads are free running\n", 
            0,0,0,0},
			XDD_FUNC_INVISIBLE},
    {"nomemlock", "nomlock",
            xddfunc_nomemlock,     
            1,  
            "  -nomemlock\n",  
            {"    Will set not lock memory\n", 
            0,0,0,0},
			0},
    {"noordering", "no",
            xddfunc_noordering,     
            1,  
            "  -noordering [target <target#>]\n",  
            {"    Will turn off Loose and Strict Ordering of QThreads for all targets or a specified target. Default is No Ordering\n", 
             "    see also: -strictordering or -looseordering\n",
			0,0,0},
			0},
    {"noproclock", "noplock",
            xddfunc_noproclock,     
            1,  
            "  -noproclock\n",  
            {"    Will set not lock process into memory\n", 
            0,0,0,0},
			0},
    {"numreqs", "nr",
            xddfunc_numreqs, 
            1,  
            "  -numreqs [target <target#>] <#>\n",  
            {"    Specifies the number of requests of request_size to issue\n", 
            0,0,0,0},
			0},
    {"operationdelay", "opdelay",       
            xddfunc_operationdelay,      
            1,  
            "  -operationdelay #seconds\n",  
            {"    Specifies the number of seconds to delay between individual operations regardless of thorttle\n", 
            0,0,0,0},
			XDD_FUNC_INVISIBLE},
    {"operation", "op",
            xddfunc_operation,  
            1,  
            "  -operation [target <target#>] read|write|noop\n",   
            {"   The operation is either 'read', 'write', 'noop' -or- 'target # read', 'target # write', or 'target # noop'\n", 
             "   The 'target # <op>' will cause the specified target to perform the specified operations\n",
             0,0,0},
			0},
    {"output", "o",
            xddfunc_output,     
            1,  
            "  -output <filename>\n",  
            {"    Will direct all output to the specified file\n", 
            0,0,0,0},
			0},
    {"outputformat", "ofmt",
            xddfunc_output_format,     
            1,  
            "  -outputformat <add | new> <formatidstring>\n",  
            {"    Specify a 'new' output format or 'add' variables to the existing output format.\n",
             "    The existing output format specification is as follows:\n",
             DEFAULT_OUTPUT_FORMAT_STRING,
            0,0},
			0},
    {"passdelay", "pdelay",       
            xddfunc_passdelay,      
            1,  
            "  -passdelay #seconds\n",  
            {"    Specifies the number of seconds to delay between passes\n", 
            0,0,0,0},
			0},
    {"passes", "p",
            xddfunc_passes,     
            1,  
            "  -passes #\n",  
            {"    Specifies the number of times to read mbytes -or- the number of times to issue 'numreqs' requests\n", 
            0,0,0,0},
			0},
    {"passoffset",  "po",
            xddfunc_passoffset, 
            1,  
            "  -passoffset [target <target#>] <#blocks>\n",  
            {"    Specifies the number of blocks to offset between passes\n", 
            0,0,0,0},
			0},
    {"percentcpu", "pcpu",
            xddfunc_percentcpu,     
            1,  
            "  -percentcpu <absolute | releative>\n",  
            {"    Specifies the meaning of the CPU utilization values\n", 
             "      where 'absolute' is the amount of CPU time divided by the wall-clock time\n",
             "            'relative' is the amount of user or system time divided by user+system time\n",
             "    Default is 'abosolute'\n",
             0},
			XDD_FUNC_INVISIBLE},
    {"preallocate", "pa",
            xddfunc_preallocate,
            1,  
            "  -preallocate [target <target#>] <#bytes>\n",  
            {"    Will preallocate # bytes before writing a file.\n", 
            0,0,0,0},
			0},
    {"processlock", "plock",      
            xddfunc_processlock,      
            1,  
            "  -processlock\n",  
            {"    Will lock the process in memory\n", 
            0,0,0,0},
			0},
    {"processor", "pt",
            xddfunc_processor, 
            1,  
            "  -processor target# processor#\n",  
            {"    Specifies which processor xdd should run on for a particular target\n",
             "    Requires the processor number and the associated target number\n",
             0,0,0},
			0},
    {"queuedepth", "qd",
            xddfunc_queuedepth, 
            1,  
            "  -queuedepth #cmds\n",   
            {"    Specifies the number of commands to queue on the target\n", 
            0,0,0,0},
			0},
    {"qthreadinfo", "qtinfo",
            xddfunc_qthreadinfo,
            1,  
            "  -qthreadinfo\n",  
            {"    Will print out information about each of the queue threads \n", 
            0,0,0,0},
			0},
    {"randomize", "rand",
            xddfunc_randomize,  
            1,  
            "  -randomize [target <target#>]\n",   
            {"    Will re-randomize the seek list between passes\n", 
            0,0,0,0},
			0},
    {"readafterwrite","raw",
            xddfunc_readafterwrite,
            1,  
            "  -readafterwrite [target #] trigger <stat | mp> | lag <#> | reader <hostname> | port <#>\n",  
            {"    Specifies a reader and writer for doing read-after-writes to a single target", 
            0,0,0,0},
			XDD_FUNC_INVISIBLE},
    {"reallyverbose", "rv",
            xddfunc_reallyverbose, 
            1,  
            "  -reallyverbose\n",  
            {"    Displays way more information than you need to know", 
            0,0,0,0},
			XDD_FUNC_INVISIBLE},
    {"recreatefiles",  "recreate",
            xddfunc_recreatefiles,  
            1,  
            "  -recreatefiles [target <target#>]\n",  
            {"    Will recreate a new target file on each pass\n", 
            0,0,0,0},
			0},
    {"reopen",  "reop",
            xddfunc_reopen,  
            1,  
            "  -reopen [target <target#>]\n",  
            {"    Will cause the target file to be closed at the end of each pass and re-opened at the beginning of each pass\n", 
            0,0,0,0},
			0},
    {"reportthreshold","rept",
            xddfunc_report_threshold,
            1,
            "  -reportthreshold [target #] <#.#>\n",  
            {"    The report threshhold will report the byte location of the operation that exceeded the specified threshold time.\n", 
            0,0,0,0},
			0},
    {"reqsize", "rs",
            xddfunc_reqsize,    
            1,  
            "  -reqsize [target <target#>] <#blocks>\n",  
            {"    Specifies the number of 'blocks' per operation where the block size is defined by the '-blocksize' option\n",
             "    If the request size is specified in the format '-reqsize target <targetnumber> <size>' \n",
             "    then the speficied target is assigned the specified request size\n",
             0,0},
			0},
    {"restart", "rst",
            xddfunc_restart,    
            1,  
            "  -restart [target <target#>] enable | frequency <seconds> | file <name_of_restart_file> | offset <offset_in_bytes>\n",  
            {"    if just 'enable' is specified then the restart will start monitoring an end-to-end operation\n",
             "    if the name of the restart file is specified then a restart operation is attempted \n",
             "    if the offset is specified then a restart from that point is initiated regardless of what the restart file indicates\n",
             "    The 'frequency' is the number of seconds between monitor events and defaults to 1\n",
             0},
			0},
    {"retry", "ret",
            xddfunc_retry,
            1,  
            "  -retry [target <target#>] <#retries\n",  
	{"    Will retry an operation '#retries' times before continuing to next operation\n", 
            0,0,0,0},
			XDD_FUNC_INVISIBLE},
    {"roundrobin",  "rr",
            xddfunc_roundrobin, 
            1,  
            "  -roundrobin # or 'all'\n",  
            {"    Specifies that the threads for multiple targets should be distributed across # processors\n",
             "    If the word 'all' is specified then all available processors will be used.\n",
             0,0,0},
			0},
    {"runtime", "rt",
            xddfunc_runtime,    
            1,  
            "  -runtime #seconds\n",   
            {"    Specifies the number of seconds the entire run should take.\n",
             "    This will set the pass count to infinite and will cause the run to end after the specified number of seconds.\n",
             0,0,0},
			0},
    {"rwratio", "rw",
            xddfunc_rwratio, 
            1,  
            "  -rwratio [target <target#>] <ratio>\n",  
            {"     Specifies the percentage of read operations to write operations.\n",
             "     A value of 0 will result in no read operations and 100% write operations.\n",
             "     A value of 100 will result in 100% read operations and no write operations.\n",
             "     Values between 0 and 100 will adjust the read and write operations accordingly.\n",
             "     Values less than 0 or greater than 100 will result in an error.\n"},
			0},
    {"seek",  "s",
            xddfunc_seek,       
            1,  
            "  -seek [target <target#>] save <filename> | load <filename> | disthist #buckets | seekhist #buckets | sequential | random | range #blocks | stagger | interleave #blocks | seed # | none\n",  
            {"    -seek 'save <filename>' will save the seek list in the file specified\n\
    -seek 'load <filename>' will load the seek list from the file specified\n\
    -seek 'disthist #buckets' will display a 'seek distance' histogram using the specified number of 'buckets'\n\
    -seek 'seekhist #buckets' will display a 'seek location' histogram using the specified number of 'buckets'\n\
    -seek 'sequential' will generate sequential seeks - this is the default \n",
             "    -seek 'random' will generate random seeks over the range specified in -range \n\
    -seek 'range #' is the range of blocksized-blocks over which to operate\n\
    -seek 'stagger' specifies a staggered sequential access over 'range'\n\
    -seek 'interleave #' specifies the number of blocksized blocks to interleave into the access pattern\n\
    -seek 'seed #' specifies a seed to use when generating random numbers\n\
    -seek 'none' do not seek - retransfer the same block each time \n",
                0,0,0},
			0},
    {"setup", "setup",
            xddfunc_setup,      
            1,  
            "  -setup filename\n",  
            {"    Specifies a file contains more command-line options\n", 
            0,0,0,0},
			0},
    {"scsigeneric", "sgio",
            xddfunc_sgio,      
            1,  
            "  -sgio\n",  
            {"    Will use SCSI Generic I/O <linux only> - only necessary if SG device is not /dev/sgX\n", 
            0,0,0,0},
			0},
    {"sharedmemory","shm",
            xddfunc_sharedmemory,
            1,  
            "  -sharedmemory [target <target#>]\n",   
            {"    Will use a shared memory segment instead of the normal malloc/valloc for the I/O buffer.\n", 
            0,0,0,0},
			0},
    {"singleproc",  "sp",
            xddfunc_singleproc, 
            1,  
            "  -singleproc #\n",  
            {"    Specifies that all xdd processes for multiple targets should run on processor #\n",
             "    Requires the processor number to run on\n",
             0,0,0},
			0},
    {"startdelay", "sd",
            xddfunc_startdelay, 
            1,  
            "  -startdelay [target <target#>]#.#seconds\n",   
            {"    Specifies the number of seconds to delay before starting.\n", 
             "    If the startdelay is specified in the format '-startdelay target <targetnumber> <seconds>'",
             "    then the speficied target is assigned the specified start delay\n",
            0,0},
			0},
    {"startoffset", "sto",
            xddfunc_startoffset,
            1,  
            "  -startoffset [target <target#>] #\n",  
            {"    Specifies the disk offset in 'blocksize'-byte blocks to begin the operation.\n", 
            0,0,0,0},
			0},
    {"starttime", "stime",
            xddfunc_starttime,  
            1,  
            "  -starttime #seconds\n",  
            {"    Specifies the global start time in seconds - to be used for distributed, synchronize runs \n", 
            0,0,0,0},
			0},
    {"starttrigger","st",
            xddfunc_starttrigger,
            1, 
            "  -starttrigger <target#> <target#> <<time|op|percent|mbytes|kbytes> #>\n",   
            {" ", 0,0,0,0},
			0},
    {"stoponerror", "soe",
            xddfunc_stoponerror,
            1,  
            "  -stoponerror\n",   
            {"    Will cause all targets and threads to stop on the first error encountered by *any* target", 0,0,0,0},
			0},
    {"stoptrigger", "et",
            xddfunc_stoptrigger,
            1,  
            "  -stoptrigger <target#> <target#> <<time|op|percent|mbytes|kbytes> #>\n",   
            {" ", 0,0,0,0},
			0},
    {"strictordering", "so",
            xddfunc_strictordering,     
            1,  
            "  -strictordering [target <target#>]\n",  
            {"    Will enforce Strict Ordering on QThreads for all targets or a specified target. Default is No Ordering\n", 
             "    see also: -looseordering or -noordering\n",
			0,0,0},
			0},
    {"syncio", "sio",
            xddfunc_syncio,     
            1,  
            "  -syncio #\n",   
            {"    Will synchonize every #th I/O operation.\n", 
            0,0,0,0},
			0},
    {"syncwrite", "sw",
            xddfunc_syncwrite,  
            1,  
            "  -syncwrite [target <target#>]\n",   
            {"    Will cause all write buffers to flush to disk.\n", 
            0,0,0,0},
			0},
    {"target", "target",
            xddfunc_target,    
            1,  
            "  -target filename\n",  
            {"    Specifies the device or file to perform operation on\n",
             "    Required parameter - no default unless -targets option used\n",
             "    See also: -targets\n",
             0,0},
			0},
    {"targetdir",   "td",
            xddfunc_targetdir,  
            1,  
            "  -targetdir [target <target#>] <directory_name>\n",  
            {"    Specifies the target directory that specific target or all targets live in\n", 
            0,0,0,0},
			0},
    {"targetoffset","to",
            xddfunc_targetoffset,
            1,  
            "  -targetoffset #",  
            {"    Specify an offset in 'blocksize'-byte blocks that is multiplied by the target number and added to each target's starting location\n", 
            0,0,0,0},
			0},
    {"targets", "targets",
            xddfunc_targets,    
            1,  
			"  -targets # filename filename filename... -or- -targets -# filename\n",  
            {"    Specifies the devices or files to perform operation on\n",
             "    Requires number of files plus list of devices and/or file names to use as targets \n",
			 "    If the number of devices is negative, then duplicate the single device and/or file name # times\n",
             0,0},
			0},
    {"targetstartdelay", "tsd",
            xddfunc_targetstartdelay, 
            1,  
            "  -targetstartdelay #.#seconds\n",  
            {"    Specify a delay in seconds that is multiplied by the target number and added to each target's start time\n",
            0,0,0,0},
			0},
    {"throttle", "throt",
            xddfunc_throttle,   
            1,  
            "  -throttle [target <target#>] <ops|bw|var> <#.#ops | #.#MB/sec | #.#var>\n",   
            {"    -throttle <ops|bw|var> #.# will cause each target to run at the IOPS or bandwidth specified as #.#\n",
             "    -throttle target N ops #.# will cause the target number N to run at the number of ops per second specified as #.#\n",
             "    -throttle target N bw #.#  will cause the target number N to run at the bandwidth specified as #.#\n",
             "    -throttle target N delay #.#  specifies that there should be # seconds of delay between each operation.\n    -throttle target N var #.#  specifies that the BW or IOPS rate should vary by the amount specified.\n",
             0},
			0},
    {"timelimit", "tl",
            xddfunc_timelimit,  
            1,  
            "  -timelimit [target <target#>] <#seconds>\n",   
            {"    Specifies the maximum number of seconds for each pass\n", 
            0,0,0,0},
			0},
    {"timerinfo", "ti",
            xddfunc_timerinfo,  
            1,  
            "  -timerinfo\n",  
            {"    Will print out overhead information for various timer functions used\n", 
            0,0,0,0},
			XDD_FUNC_INVISIBLE},
    {"timeserver", "tsvr",
            xddfunc_timeserver, 
            1,  
            "  -timeserver <host hostname | port # | bounce #>\n",  
            {"    -timeserver 'host' will use [hostname] as the time server in a distributed, synchronized run\n", 
             "    -timeserver 'port' will use # as the port number to use to talk to the time server host\n",
             "    -timeserver 'bouce' will use # as the bounce count to use to talk to ping the time server host\n",
             0,0},
			0},
    {"timestamps", "ts",
            xddfunc_timestamp,  
            1,  
            "  -ts [target <target#>] summary|detailed|wrap|oneshot|size #|append|output <filename>|dump <filename>|triggertime <seconds>|triggerop <op#>\n",   
            {"    -ts  'summary' will turn on time stamping with summary reporting option\n\
    -ts  'detailed'  will turn on time stamping with detailed reporting option\n\
    -ts  'wrap'  will cause the timestamp buffer to wrap after N timestamp entries are used. Should be used in conjunction with -ts size.\n\
    -ts  'oneshot' will stop time stamping after all timestamp entries are used.\n\
    -ts  'size #'  will restrict the size of the timestamp buffer to # entries.\n",
             "    -ts  'triggertime #seconds'  will restrict the size of the timestamp buffer to # entries.\n\
    -ts  'triggerop op#'  will restrict the size of the timestamp buffer to # entries.\n\
    -ts  'append'  will append output to existing output file.\n\
    -ts  'output filename' will print the output to file 'filename'. Default output is stdout\n\
    -ts  'dump filename'  will turn on time stamping and dump a binary time stamp file to 'filename'\n\
    Default is no time stamping.\n",
              0,0,0},
			0},
    {"unverbose", "unv",
            xddfunc_unverbose,    
            1,  
            "  -unverbose\n",  
            {"    Will turn off verbose mode \n", 
            0,0,0,0},
			XDD_FUNC_INVISIBLE},
    {"verbose", "verbose",
            xddfunc_verbose,    
            1,  
            "  -verbose\n",  
            {"    Will print out statistics on each pass \n", 
            0,0,0,0},
			0},
    {"verify", "verify",
            xddfunc_verify,     
            1,  
            "  -verify [target <target#>] location|contents\n",   
            {"    -verify  'location'  will verify the block location is correct\n",
             "    -verify  'contents' will verify the contents of the data buffer read is the same as the specified data pattern\n",
             0,0,0},
			0},
    {"version", "ver",
            xddfunc_version,     
            1,  
            "  -version\n",   
            {"    Will print out the version number of this program\n",
             0,0,0,0},
			0},
    {"writeafterread", "war",
            xddfunc_endtoend,
            1,
            "  -writeafterread or war\n",   
            {"  see 'endtoend or e2e'\n",
            0,0,0,0},
			XDD_FUNC_INVISIBLE},
    {0,0,0,0,0,{0,0,0,0,0},0}
}; // This is the end of the command line options definitions

 
/*
 * Local variables:
 *  indent-tabs-mode: t
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ts=4 sts=4 sw=4 noexpandtab
 */
