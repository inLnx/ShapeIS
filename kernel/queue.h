/*
 * Copyright (c) 2021 - 2022, the pranaOS Developers & Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <listing/list.h>
#include <stdint.h>

/**
 * @brief struct queue
 * 
 */
struct queue {
    struct list_head *qhead;
};

/**
 * @brief queue push
 * 
 * @param q 
 * @param data 
 */
void queue_push(struct queue *q, void *data);

/**
 * @brief queue pop 
 * 
 * @param q 
 * @return void* 
 */
void *queue_pop(struct queue *q);