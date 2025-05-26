/*
** EPITECH PROJECT, 2021
** tree
** File description:
** tree
*/

#include "engine_building.h"
#include <stdlib.h>

void tree_create(world_building_t *building, sfVector2f pos)
{
    tree_t *tree = malloc(sizeof(tree_t));

    if (tree == NULL)
        return;
    building_create(&building->all_building, &tree->building_data,
        TREE_BUILDING, pos);
    tree->building_data.passable = false;
    my_vector_add(&building->tree, tree);
}

void tree_init(vector_t *tree_vec)
{
    my_vector_init(tree_vec);
}

void tree_update(world_building_t *building)
{
    tree_t *current_tree;

    for (int i = 0; i != building->tree.size; i++) {
        current_tree = my_vector_get(&building->tree, i);
    }
}

void tree_render(sfRenderWindow *window, world_building_t *building)
{
}

void tree_destroy(world_building_t *world_building)
{
    tree_t *current_tree;

    for (int i = 0; i != world_building->tree.size; i++) {
        current_tree = my_vector_get(&world_building->tree, i);
        building_delete(&world_building->all_building,
            &current_tree->building_data);
        free(current_tree);
    }
    my_vector_free(&world_building->tree);
}