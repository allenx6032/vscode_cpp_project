/*
** EPITECH PROJECT, 2024
** Nom du projet
** File description:
** Description du projet
*/

#include <SFML_Graphics.h>
#include <SFML_System.h>
#include "my_entity.h"
#include "my_utils.h"

entity_t *pos_sca(
    entity_t *entity,
    sfVector2f pos,
    sfVector2f scale
)
{
    POS(entity, pos.x, pos.y);
    SCA(entity, scale.x, scale.y);
    return entity;
}

entity_t *img(
    render_element_t el,
    sfTexture *texture,
    sfVector2f pos,
    sfVector2f sca
)
{
    entity_t *entity = create_entity(el, NULL, NULL, NULL);

    APPLY_TEXTURE(GET_SPRITE(entity), texture);
    pos_sca(entity, pos, sca);
    return entity;
}

entity_t *fetch_entity_by_id(scene_t *scene, int id)
{
    linked_list_t *temp = scene->entities;
    entity_t *cur = NULL;

    while (temp != NULL) {
        cur = temp->data;
        if (cur->id == id)
            return cur;
        temp = temp->next;
    }
    return NULL;
}

linked_list_t *fetch_entities_by_id(scene_t *scene, int id)
{
    linked_list_t *temp = scene->entities;
    entity_t *cur = NULL;
    linked_list_t *entities = NULL;

    while (temp != NULL) {
        cur = temp->data;
        if (cur->id == id)
            entities = push_front(entities, cur);
        temp = temp->next;
    }
    return entities;
}
