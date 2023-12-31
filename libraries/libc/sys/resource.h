/**
 * @file resource.h
 * @author Krisna Pranav
 * @brief resource
 * @version 6.0
 * @date 2023-07-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once 

#include <sys/cdefs.h>
#include <sys/time.h>


__BEGIN_DECLS

struct rusage 
{
    struct timeval ru_utime;
    struct timeval ru_stime;
    long ru_maxrss;
    long ru_ixrss;
    long ru_idrss;
    long ru_isrss;
    long ru_minflt;
    long ru_majflt;
    long ru_nswap;
    long ru_inblock;
    long ru_oublock;
    long ru_msgsnd;
    long ru_msgrcv;
    long ru_nsignals;
    long ru_nvcsw;
    long ru_nivcsw;
}; // struct rusage

#define RUSAGE_SELF 1
#define RUSAGE_CHILDREN 2

/**
 * @param who 
 * @param usage 
 * @return int 
 */
int getrusage(int who, struct rusage* usage);

__END_DECLS
