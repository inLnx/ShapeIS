/**
 * @file locale.h
 * @author Krisna Pranav
 * @brief locale
 * @version 1.0
 * @date 2023-07-07
 * 
 * @copyright Copyright (c) 2021 - 2023 pranaOS Developers, Krisna Pranav
 * 
 */

#pragma once

#include <sys/cdefs.h>

__BEGIN_DECLS

/// @brief LC
enum {
    LC_ALL,
    LC_NUMERIC,
    LC_CTYPE,
    LC_COLLATE,
    LC_TIME,
    LC_MONETARY,
};

/// @brief lconv
struct lconv {
    char* decimal_point;
    char* thousands_sep;
    char* grouping;
    char* int_curr_symbol;
    char* currency_symbol;
    char* mon_decimal_point;
    char* mon_thousands_sep;
    char* mon_grouping;
    char* positive_sign;
    char* negative_sign;
    char int_frac_digits;
    char frac_digits;
    char p_cs_precedes;
    char p_sep_by_space;
    char n_cs_precedes;
    char n_sep_by_space;
    char p_sign_posn;
    char n_sign_posn;
    char int_p_cs_precedes;
    char int_p_sep_by_space;
    char int_n_cs_precedes;
    char int_n_sep_by_space;
    char int_p_sign_posn;
    char int_n_sign_posn;
};

/**
 * @return struct lconv* 
 */
struct lconv* localeconv();

/**
 * @param category 
 * @param locale 
 * @return char* 
 */
char* setlocale(int category, const char* locale);

__END_DECLS
