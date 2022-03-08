/*
 * Copyright (c) 2021 - 2022, the pranaOS Developers & Krisna Pranav
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <sys/cdefs.h>

/**
 * @brief containerof
 * 
 */
#define container_of(ptr, type, member) ({ \
    const typeof( ((type *)0)->member ) \
    *__mptr = (ptr); \
    (type *)( (char *)__mptr - offsetof(type,member) ); })

/**
 * @brief LIST1, LIST2
 * 
 */
#define LIST1 NULL
#define LIST2 NULL

/**
 * @brief list head
 * 
 */
struct list_head {
	struct list_head *next, *prev;
};

/**
 * @brief list head init 
 * 
 */
#define LIST_HEAD_INIT(name) \
	{                        \
		&(name), &(name)     \
	}

/**
 * @brief list head
 * 
 */
#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

/**
 * @brief init list head
 * 
 * @param list 
 */
static inline void init_list(struct list_head *list) {
	list->next = list;
	list->prev = list;
}

/**
 * @brief list add value
 * 
 * @param new 
 * @param prev 
 * @param next 
 * @return true 
 * @return false 
 */
static inline bool __list_add_valid(__unused struct list_head *new,
									__unused struct list_head *prev,
									__unused struct list_head *next) {
	return true;
}

/**
 * @brief list delete entry valid 
 * 
 * @param entry 
 * @return true 
 * @return false 
 */
static inline bool __list_del_entry_valid(struct list_head *entry) {
	return entry->next != LIST1 && entry->prev != LIST2;
}

/**
 * @brief list add
 * 
 * @param new 
 * @param prev 
 * @param next 
 */
static inline void __list_add(struct list_head *new,
							  struct list_head *prev,
							  struct list_head *next) {
	if (!__list_add_valid(new, prev, next))
		return;

	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

/**
 * @brief list add
 * 
 * @param new 
 * @param head 
 */
static inline void list_add(struct list_head *new, struct list_head *head) {
	__list_add(new, head, head->next);
}

/**
 * @brief list add tail
 * 
 * @param new 
 * @param head 
 */
static inline void list_add_tail(struct list_head *new, struct list_head *head) {
	__list_add(new, head->prev, head);
}

/**
 * @brief list delete
 * 
 * @param prev 
 * @param next 
 */
static inline void __list_del(struct list_head *prev, struct list_head *next) {
	next->prev = prev;
	prev->next = next;
}

/**
 * @brief list delete clearprev
 * 
 * @param entry 
 */
static inline void __list_del_clearprev(struct list_head *entry) {
	__list_del(entry->prev, entry->next);
	entry->prev = NULL;
}

/**
 * @brief list delete entry
 * 
 * @param entry 
 */
static inline void __list_del_entry(struct list_head *entry) {
	if (!__list_del_entry_valid(entry))
		return;

	__list_del(entry->prev, entry->next);
}

/**
 * @brief list delete
 * 
 * @param entry 
 */
static inline void list_del(struct list_head *entry)
{
	__list_del_entry(entry);
	entry->next = LIST1;
	entry->prev = LIST2;
}

/**
 * @brief list replace
 * 
 * @param old 
 * @param new 
 */
static inline void list_replace(struct list_head *old,
								struct list_head *new) {
	new->next = old->next;
	new->next->prev = new;
	new->prev = old->prev;
	new->prev->next = new;
}

/**
 * @brief list replace init
 * 
 * @param old 
 * @param new 
 */
static inline void list_replace_init(struct list_head *old,
									 struct list_head *new) {
	list_replace(old, new);
	init_list(old);
}

/**
 * @brief list swap
 * 
 * @param entry1 
 * @param entry2 
 */
static inline void list_swap(struct list_head *entry1,
							 struct list_head *entry2) {
	struct list_head *pos = entry2->prev;

	list_del(entry2);
	list_replace(entry1, entry2);
	if (pos == entry1)
		pos = entry2;
	list_add(entry1, pos);
}

/**
 * @brief list delete init
 * 
 * @param entry 
 */
static inline void list_del_init(struct list_head *entry) {
	__list_del_entry(entry);
	init_list(entry);
}

/**
 * @brief list move
 * 
 * @param list 
 * @param head 
 */
static inline void list_move(struct list_head *list, struct list_head *head) {
	__list_del_entry(list);
	list_add(list, head);
}

/**
 * @brief list move tail
 * 
 * @param list 
 * @param head 
 */
static inline void list_move_tail(struct list_head *list,
								  struct list_head *head) {
	__list_del_entry(list);
	list_add_tail(list, head);
}

/**
 * @brief list bulk move tail
 * 
 * @param head 
 * @param first 
 * @param last 
 */
static inline void list_bulk_move_tail(struct list_head *head,
									   struct list_head *first,
									   struct list_head *last) {
	first->prev->next = last->next;
	last->next->prev = first->prev;

	head->prev->next = first;
	first->prev = head->prev;

	last->next = head;
	head->prev = last;
}

/**
 * @brief list is first
 * 
 * @param list 
 * @param head 
 * @return int 
 */
static inline int list_is_first(const struct list_head *list,
								const struct list_head *head) {
	return list->prev == head;
}

/**
 * @brief list is last
 * 
 * @param list 
 * @param head 
 * @return int 
 */
static inline int list_is_last(const struct list_head *list,
							   const struct list_head *head) {
	return list->next == head;
}

/**
 * @brief list empty 
 * 
 * @param head 
 * @return int 
 */
static inline int list_empty(const struct list_head *head) {
	return head->next == head;
}

/**
 * @brief list empty careful
 * 
 * @param head 
 * @return int 
 */
static inline int list_empty_careful(const struct list_head *head) {
	struct list_head *next = head->next;
	return (next == head) && (next == head->prev);
}

/**
 * @brief list rotate left
 * 
 * @param head 
 */
static inline void list_rotate_left(struct list_head *head) {
	struct list_head *first;

	if (!list_empty(head))
	{
		first = head->next;
		list_move_tail(first, head);
	}
}

/**
 * @brief list rotate to front
 * 
 * @param list 
 * @param head 
 */
static inline void list_rotate_to_front(struct list_head *list,
										struct list_head *head) {
	list_move_tail(head, list);
}

/**
 * @brief list is singular
 * 
 * @param head 
 * @return int 
 */
static inline int list_is_singular(const struct list_head *head) {
	return !list_empty(head) && (head->next == head->prev);
}

/**
 * @brief list cut position
 * 
 * @param list 
 * @param head 
 * @param entry 
 */
static inline void __list_cut_position(struct list_head *list,
									   struct list_head *head, struct list_head *entry) {
	struct list_head *new_first = entry->next;
	list->next = head->next;
	list->next->prev = list;
	list->prev = entry;
	entry->next = list;
	head->next = new_first;
	new_first->prev = head;
}

/**
 * @brief list cut position callback
 * 
 * @param list 
 * @param head 
 * @param entry 
 */
static inline void list_cut_position(struct list_head *list,
									 struct list_head *head, struct list_head *entry) {
	if (list_empty(head))
		return;
	if (list_is_singular(head) &&
		(head->next != entry && head != entry))
		return;
	if (entry == head)
		init_list(list);
	else
		__list_cut_position(list, head, entry);
}

/**
 * @brief list cut before
 * 
 * @param list 
 * @param head 
 * @param entry 
 */
static inline void list_cut_before(struct list_head *list,
								   struct list_head *head,
								   struct list_head *entry) {
	if (head->next == entry)
	{
		init_list(list);
		return;
	}
	list->next = head->next;
	list->next->prev = list;
	list->prev = entry->prev;
	list->prev->next = list;
	head->next = entry;
	entry->prev = head;
}

/**
 * @brief list splice
 * 
 * @param list 
 * @param prev 
 * @param next 
 */
static inline void __list_splice(const struct list_head *list,
								 struct list_head *prev,
								 struct list_head *next) {
	struct list_head *first = list->next;
	struct list_head *last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

/**
 * @brief list splice
 * 
 * @param list 
 * @param head 
 */
static inline void list_splice(const struct list_head *list,
							   struct list_head *head) {
	if (!list_empty(list))
		__list_splice(list, head, head->next);
}

/**
 * @brief list splice tail
 * 
 * @param list 
 * @param head 
 */
static inline void list_splice_tail(struct list_head *list,
									struct list_head *head) {
	if (!list_empty(list))
		__list_splice(list, head->prev, head);
}

/**
 * @brief list splice init 
 * 
 * @param list 
 * @param head 
 */
static inline void list_splice_init(struct list_head *list,
									struct list_head *head) {
	if (!list_empty(list))
	{
		__list_splice(list, head, head->next);
		init_list(list);
	}
}

/**
 * @brief list splice tail init 
 * 
 * @param list 
 * @param head 
 */
static inline void list_splice_tail_init(struct list_head *list,
										 struct list_head *head) {
	if (!list_empty(list))
	{
		__list_splice(list, head->prev, head);
		init_list(list);
	}
}

/**
 * @brief list entry
 * 
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * @brief list first entry
 * 
 */
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

/**
 * @brief list last entry
 * 
 */
#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)

#define list_first_entry_or_null(ptr, type, member) ({        \
	struct list_head *head__ = (ptr);                         \
	struct list_head *pos__ = head__->next;                   \
	pos__ != head__ ? list_entry(pos__, type, member) : NULL; \
})

#define list_last_entry_or_null(ptr, type, member) ({         \
	struct list_head *head__ = (ptr);                         \
	struct list_head *pos__ = head__->prev;                   \
	pos__ != head__ ? list_entry(pos__, type, member) : NULL; \
})

#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)

#define list_prev_entry(pos, member) \
	list_entry((pos)->member.prev, typeof(*(pos)), member)


#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)


#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)


#define list_for_each_safe(pos, n, head)                   \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		 pos = n, n = pos->next)


#define list_for_each_prev_safe(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev;   \
		 pos != (head);                       \
		 pos = n, n = pos->prev)


#define list_for_each_entry(pos, head, member)               \
	for (pos = list_first_entry(head, typeof(*pos), member); \
		 &pos->member != (head);                             \
		 pos = list_next_entry(pos, member))


#define list_for_each_entry_reverse(pos, head, member)      \
	for (pos = list_last_entry(head, typeof(*pos), member); \
		 &pos->member != (head);                            \
		 pos = list_prev_entry(pos, member))


#define list_prepare_entry(pos, head, member) \
	((pos) ?: list_entry(head, typeof(*pos), member))


#define list_for_each_entry_continue(pos, head, member) \
	for (pos = list_next_entry(pos, member);            \
		 &pos->member != (head);                        \
		 pos = list_next_entry(pos, member))


#define list_for_each_entry_continue_reverse(pos, head, member) \
	for (pos = list_prev_entry(pos, member);                    \
		 &pos->member != (head);                                \
		 pos = list_prev_entry(pos, member))


#define list_for_each_entry_from(pos, head, member) \
	for (; &pos->member != (head);                  \
		 pos = list_next_entry(pos, member))

#define list_for_each_entry_from_reverse(pos, head, member) \
	for (; &pos->member != (head);                          \
		 pos = list_prev_entry(pos, member))


#define list_for_each_entry_safe(pos, n, head, member)       \
	for (pos = list_first_entry(head, typeof(*pos), member), \
		n = list_next_entry(pos, member);                    \
		 &pos->member != (head);                             \
		 pos = n, n = list_next_entry(n, member))


#define list_for_each_entry_safe_continue(pos, n, head, member) \
	for (pos = list_next_entry(pos, member),                    \
		n = list_next_entry(pos, member);                       \
		 &pos->member != (head);                                \
		 pos = n, n = list_next_entry(n, member))


#define list_for_each_entry_safe_from(pos, n, head, member) \
	for (n = list_next_entry(pos, member);                  \
		 &pos->member != (head);                            \
		 pos = n, n = list_next_entry(n, member))


#define list_for_each_entry_safe_reverse(pos, n, head, member) \
	for (pos = list_last_entry(head, typeof(*pos), member),    \
		n = list_prev_entry(pos, member);                      \
		 &pos->member != (head);                               \
		 pos = n, n = list_prev_entry(n, member))


/**
 * @brief list safe reset next 
 * 
 */
#define list_safe_reset_next(pos, n, member) \
	n = list_next_entry(pos, member)

/**
 * @brief list count
 * 
 * @param head 
 * @return int 
 */
static inline int list_count(const struct list_head *head) {
	int count = 0;
	struct list_head *iter;
	list_for_each(iter, head)
		count++;
	return count;
}

/**
 * @brief list check
 * 
 * @param entry 
 * @return int 
 */
static inline int list_check(const struct list_head *entry) {
	return entry->prev == LIST1 && entry->next == LIST2;
}

/**
 * @brief list last entry
 * 
 */
#define list_is_last_entry(pos, head, member) list_is_last(&pos->member, head)

/**
 * @brief list head entry
 * 
 */
#define list_is_head_entry(pos, head, member) (&pos->member == head)