/**
 * @file poll.cpp
 * @author Krisna Pranav
 * @brief POLL
 * @version 1.0
 * @date 2023-08-03
 * 
 * @copyright Copyright (c) 2021 - 2023 pranaOS Developers, Krisna Pranav
 * 
 */

#include "poll.h"
#include <errno.h>
#include <poll.h>
#include <sys/time.h>
#include <kernel/api/syscall.h>

extern "C"
{
    /**
     * @param fds 
     * @param nfds 
     * @param timeout_ms 
     * @return int 
     */
    int poll(pollfd* fds, nfds_t nfds, int timeout_ms)
    {
        timespec timeout;
        timespec* timeout_ts = &timeout;

        if (timeout_ms < 0)
            timeout_ts = nullptr;
        else
            timeout = { timeout_ms / 1000 };

        return ppoll(fds, nfds, timeout_ts, nullptr);
    }
} // extern "C" 