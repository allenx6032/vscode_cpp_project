/*
** EPITECH PROJECT, 2024
** linked
** File description:
** list
*/

#pragma once

#include <stdbool.h>
#include <unistd.h>

typedef struct list_s list_t;
typedef struct node_s node_t;

struct list_s {
    node_t *head;
    node_t *tail;
    size_t size; // Do not modify directly
};

struct node_s {
    void *data;
    list_t *list;
    node_t *prev;
    node_t *next;
};

typedef void *(*allocator_t)();
typedef void (*callback_t)(void *);
typedef void (*applicator_t)(void *, void *);
typedef bool (*comparator_t)(void *, void *);

// creating

node_t *node_create(allocator_t allocate);
list_t *list_create(void);
void list_init(list_t *list);

// destructing

void node_destroy(node_t *node, callback_t destroy);
void node_destroy_all(node_t *node, callback_t destroy);
void list_clear(list_t *list, callback_t destroy);
void list_destroy(list_t *list, callback_t destroy);
void list_clear_free(list_t *list);
void list_destroy_free(list_t *list);

// adding

void *list_add(list_t *list, allocator_t allocate);
void *list_add_ptr(list_t *list, void *ptr);
void *list_add_copy(list_t *list, void *data, size_t size);
node_t *list_add_node(list_t *list, allocator_t allocate);
void *list_insert(list_t *list, allocator_t allocate, int index);

// removing

void *list_pop(list_t *list, int index);
bool list_remove(list_t *list, callback_t destroy, int index);
bool list_remove_free(list_t *list, int index);
int list_remove_if(list_t *list, comparator_t compare, void *right,
    callback_t destroy);
int list_remove_if_free(list_t *list, comparator_t compare, void *right);

// handling

void *list_at(list_t *list, int index);
bool list_move(list_t *from, int at, list_t *to);
bool list_apply(list_t *list, applicator_t apply, void *accumulator);
bool list_map(list_t *list, callback_t map);
bool list_has_ptr(list_t *list, void *ptr);

void *list_get_if(list_t *list, comparator_t compare, void *right);
int list_move_if(list_t *from, list_t *to, comparator_t compare, void *right);

// find

int list_find(list_t *list, comparator_t compare, void *right);
int list_find_ptr(list_t *list, void *ptr);

// utilities

void list_print(list_t *list, callback_t print);
bool list_empty(const list_t *list);

// Comparators

bool list_int_eq(void *a, void *b);
bool list_int_gt(void *a, void *b);
bool list_int_lt(void *a, void *b);

bool list_float_eq(void *a, void *b);
bool list_float_gt(void *a, void *b);
bool list_float_lt(void *a, void *b);

bool list_double_eq(void *a, void *b);
bool list_double_gt(void *a, void *b);
bool list_double_lt(void *a, void *b);

bool list_char_eq(void *a, void *b);
bool list_char_gt(void *a, void *b);
bool list_char_lt(void *a, void *b);

bool list_str_eq(void *a, void *b);
