/*
** EPITECH PROJECT, 2024
** linked
** File description:
** list_utils
*/

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include "linked_dev.h"

void node_isolate(node_t *node)
{
    if (!node)
        return;
    if (node->prev)
        node->prev->next = node->next;
    if (node->next)
        node->next->prev = node->prev;
    if (node->list) {
        if (node->list->head == node)
            node->list->head = node->next ? node->next : node->prev;
        if (node->list->tail == node)
            node->list->tail = node->prev ? node->prev : node->next;
    }
    if (node->list)
        list_dec_size(node->list);
    node->list = NULL;
    node->prev = NULL;
    node->next = NULL;
}

static void print_data(void *data, callback_t print)
{
    if (print)
        print(data);
    else
        printf("%p", data);
}

static void node_print_all(node_t *node, callback_t print)
{
    if (!node)
        return;
    while (node->prev)
        node = node->prev;
    while (node) {
        print_data(node->data, print);
        node = node->next;
        if (node) {
            printf("\u21D1\n");
            printf("\u21D3\n");
        }
    }
}

void list_print(list_t *list, callback_t print)
{
    if (!list)
        return;
    printf("--- List: %p ---\n", list);
    node_print_all(list->head, print);
    printf("--- End of list ---\n");
}

void node_link(node_t *node, ...)
{
    va_list args;
    node_t *prev = node;
    node_t *next = NULL;

    va_start(args, node);
    while (true) {
        next = va_arg(args, node_t *);
        if (!next)
            break;
        if (prev)
            prev->next = next;
        if (next)
            next->prev = prev;
        prev = next;
    }
    va_end(args);
}
