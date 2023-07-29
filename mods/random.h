/**
 * @file random.h
 * @author Krisna Pranav
 * @brief Random
 * @version 1.0
 * @date 2023-07-29
 * 
 * @copyright Copyright (c) 2021 - 2023 pranaOS Developers, Krisna Pranav
 * 
 */

#pragma once 

#include "types.h"
#include "platform.h"

#if defined(__prana__)
#   include <stdlib.h>
#endif

#if defined(__unix__)
#   include <unistd.h>
#endif

#if defined(__APPLE__)
#   include <sys/random.h>
#endif

namespace Mods
{   
    /**
     * @param buffer 
     * @param length 
     */
    inline void fill_with_random(void* buffer, size_t length)
    {
    #if defined(__prana__)
        arc4random_buf(buffer, length);
    #elif defined(OSS_FUZZ)
        (void)buffer;
    #endif
    }

    /**
     * @tparam T
    */
    template<typename T>
    template T get_random()
    {
        T t;
        fill_with_random(&t, sizeof(T));
        return t;
    }
} // namespace Mods