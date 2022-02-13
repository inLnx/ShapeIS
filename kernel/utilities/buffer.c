/*
 * Copyright (c) 2021 - 2022, the pranaOS Developers & Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stddef.h>
#include <stdint.h>
#include <buffer.h>
#include <memory/vmm.h>

/**
 * @brief circular buffer t
 * 
 */
struct circular_buf_t {
    char *buffer;
    bool full;
    size_t head;
    size_t tail;
    size_t max;
};

/**
 * @brief advance pointer
 * 
 * @param cbuf 
 */
static void advance_pointer(struct circular_buf_t *cbuf) {
    if (cbuf->full) {
        cbuf->tail = (cbuf->tail + 1) % cbuf->max;
    }

    cbuf->head = (cbuf->head + 1) % cbuf->max;

    cbuf->full = (cbuf->head == cbuf->tail);
}