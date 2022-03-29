/*
 * Copyright (c) 2021 - 2022, the pranaOS Developers & Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

    typedef char                        int8_t;
    typedef unsigned char               uint8_t;
    typedef short                       int16_t;
    typedef unsigned short              uint16_t;
    typedef int                         int32_t;
    typedef unsigned int                uint32_t;
    typedef long long int               int64_t;
    typedef unsigned long long int      uint64_t;
    typedef unsigned long long          uintptr_t;

    typedef unsigned int _kernel_dev_t;
    typedef _kernel_dev_t dev_t;

    typedef unsigned short umode_t;
    typedef long long off_t;
    typedef long off_t;
    typedef unsigned long long ino_t;
    typedef unsigned int mode_t;
    typedef long ssize_t;
    typedef unsigned long sector_t;
    typedef int pid_t;
    typedef int tid_t;
    typedef int uid_t;
    typedef int sid_t;
    typedef unsigned int sigset_t;
    typedef int idtype_t;
    typedef int id_t;
    typedef long long time_t;

    typedef int clockid_t;
    typedef int fops_t;


    typedef unsigned int fmode_t;
    typedef unsigned int nlink_t;
    
    typedef int blksize_t;
    typedef int blkcnt_t;
    
    typedef seconds_t;
    typedef unsigned int sseconds_t;    

    constexpr uint64_t divide64(uint64_t n, uint32_t base, uint32_t* r = 0) {
        uint64_t rem = n;
        uint64_t b = base;
        uint64_t res = 0, d = 1;
        uint32_t high = rem >> 32;

        if (high >= base) {
                high /= base;
                res = (uint64_t) high << 32;
                rem -= (uint64_t) (high*base) << 32;
        }

        while ((int64_t)b > 0 && b < rem) {
                b = b+b;
                d = d+d;
        }

        do {
            if (rem >= b) {
                rem -= b;
                res += d;
            }

            b >>= 1;
            d >>= 1;

            } while (d);

            if(r)
                *r = rem;
            
            return res;
    }

    constexpr ak::uint32_t operator"" _KB(unsigned long long no) {
        return no * 1024;
    }

    constexpr ak::uint32_t operator"" _MB(unsigned long long no) {
        return no * (1024_KB);
    }
    
    constexpr ak::uint32_t operator"" _GB(unsigned long long no) {
        return no * (1024_MB);
    }