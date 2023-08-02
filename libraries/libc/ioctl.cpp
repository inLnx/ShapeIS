/**
 * @file ioctl.cpp
 * @author Krisna Pranav
 * @brief ioctl
 * @version 1.0
 * @date 2023-08-02
 * 
 * @copyright Copyright (c) 2021 - 2023 pranaOS Developers, Krisna Pranav
 * 
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <kernel/api/syscall.h>

extern "C"
{
    /**
     * @param fd 
     * @param request 
     * @param ... 
     * @return int 
     */
    int ioctl(int fd, unsigned request, ...)
    {
        va_list ap;
        va_start(ap, request);
        unsigned arg = va_arg(ap, unsigned);
    } // int ioctl
} // extern