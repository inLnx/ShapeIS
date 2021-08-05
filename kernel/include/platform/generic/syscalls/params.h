/*
 * Copyright (c) 2021, Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
*/

#ifdef __i386__
#include <platform/x86/syscalls/params.h>
#elif __arm__
#include <platform/aarch32/syscalls/params.h>
#endif