/*
 * Copyright (c) 2021 - 2022, the pranaOS Developers & Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "types.h"

namespace ak {
        #define phys2virt(x) ((x) + 3_GB)
        #define virt2phys(x) ((x) - 3_GB)

        class memOperator {
        public:
            static void* memmove(void* dstptr, const void* srcptr, uint32_t size);
            static int memcmp(const void* aptr, const void* bptr, uint32_t size);
            static void* memset(void* bufptr, char value, uint32_t size);
            static void* memcpy(void* dstptr, const void* srcptr, uint32_t size);
        };
}