/**
 * @file iconv.h
 * @author Krisna Pranav
 * @brief iconv
 * @version 1.0
 * @date 2023-07-06
 * 
 * @copyright Copyright (c) 2021 - 2023 pranaOS Developers, Krisna Pranav
 * 
 */

#pragma once

#include <stddef.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

typedef void* iconv_t;

/**
 * @param tocode 
 * @param fromcode 
 * @return iconv_t 
 */
extern iconv_t iconv_open(const char* tocode, const char* fromcode);

/**
 * @param inbuf 
 * @param inbytesleft 
 * @param outbuf 
 * @param outbytesleft 
 * @return size_t 
 */
extern size_t iconv(iconv_t, char** inbuf, size_t* inbytesleft, char** outbuf, size_t* outbytesleft);

__END_DECLS