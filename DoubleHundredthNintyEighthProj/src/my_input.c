/*
** EPITECH PROJECT, 2024
** Nom du projet
** File description:
** Description du projet
*/


#include <SFML_Config.h>
#include <SFML_Graphics.h>
#include <SFML_System.h>
#include <SFML_Window.h>
#include "my_entity.h"
#include "my_utils.h"

// static sfBool allow_pos(
//     sfFloatRect rect,
//     sfVector2i pos
// )
// {
//     if (pos.x >= rect.left &&
//         pos.x <= rect.left + rect.width &&
//         pos.y <= rect.top + rect.height &&
//         pos.y >= rect.top) {
//         return sfTrue;
//     }
//     return sfFalse;
// }

sfBool is_clicked_by_mouse(entity_t *entity, engine_t *engine)
{
    static sfBool has_pressed = sfFalse;
    sfBool is_mouse_pressed = sfMouse_isButtonPressed(sfMouseLeft);
    sfFloatRect rect = sfSprite_getGlobalBounds(GET_SPRITE(entity));

    if (!has_pressed && is_mouse_pressed &&
        sfFloatRect_contains(&rect, engine->mp.x, engine->mp.y)) {
        has_pressed = sfTrue;
        return sfFalse;
    } else if (has_pressed && !is_mouse_pressed &&
        sfFloatRect_contains(&rect, engine->mp.x, engine->mp.y)) {
        has_pressed = sfFalse;
        return sfTrue;
    }
    return sfFalse;
}
