/*
** EPITECH PROJECT, 2024
** linked
** File description:
** linked_dev
*/

#pragma once

#include "linked.h"

void node_isolate(node_t *node);
void node_link(node_t *node, ...);
void list_set_head(list_t *list, node_t *node);
void list_set_tail(list_t *list, node_t *node);

void list_inc_size(list_t *list);
void list_dec_size(list_t *list);
