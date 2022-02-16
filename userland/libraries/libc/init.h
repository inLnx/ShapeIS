/*
 * Copyright (c) 2021 - 2022, the pranaOS Developers & Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>

#ifndef __wchar_t_defined
#define __wchar_t_defined
#define __need_wchar_t
#endif

typedef struct {
    intmax_t quot;
    intmax_t rem;
} imaxdiv_t;

/**
 * @brief imaxabs
 * 
 * @param val 
 * @return intmax_t 
 */
intmax_t imaxabs(intmax_t val);

/**
 * @brief imaxdiv
 * 
 * @param numer 
 * @param denom 
 * @return imaxdiv_t 
 */
imaxdiv_t imaxdiv(intmax_t numer, intmax_t denom);

/**
 * @brief strtoimax
 * 
 * @param nptr 
 * @param endptr 
 * @param base 
 * @return intmax_t 
 */
intmax_t strtoimax(const char *restrict nptr, char** endptr, int base);