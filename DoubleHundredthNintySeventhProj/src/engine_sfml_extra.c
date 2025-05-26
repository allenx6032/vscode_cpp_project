/*
** EPITECH PROJECT, 2021
** sfml_extra
** File description:
** Because a lib can't be perfect and easy, wait who said that ?
*/

// yeah sfSomething_thingCheck is sfsomething_thingcheck to respect snake case

#include "engine_rpg.h"

bool sfvector2f_isdifferent(const sfVector2f pos1, const sfVector2f pos2)
{
    if (pos1.x != pos2.x || pos1.y != pos2.y)
        return true;
    return false;
}

sfVector2f sfvector2f_fixmousepositionwindow(const sfRenderWindow *window,
    const sfVector2f pos)
{
    sfVector2f fixed_pos = (sfVector2f){
        (pos.x * VIDEO_MODE.width)
            / sfRenderWindow_getSize(window).x,
        (pos.y * VIDEO_MODE.height)
            / sfRenderWindow_getSize(window).y };

    return fixed_pos;
}

sfVector2f sfvector2f_fixmousepositionview(const sfRenderWindow *window,
    const sfView *view, const sfVector2f pos)
{
    sfVector2f fixed_pos;
    sfVector2i converted = (sfVector2i) {
        (unsigned int) pos.x,
        (unsigned int) pos.y
    };

    fixed_pos = sfRenderWindow_mapPixelToCoords(window,
        converted, view);
    return fixed_pos;
}

bool sfsprite_isvectorinside(const sfSprite *sprite, sfVector2f pos)
{
    const sfFloatRect f_rec = sfSprite_getGlobalBounds(sprite);

    if (sfFloatRect_contains(&f_rec, pos.x, pos.y))
        return true;
    return false;
}

sfVector2f sfvector2f_diff(const sfVector2f vec1, const sfVector2f vec2)
{
    sfVector2f vec = (sfVector2f) { 0, 0 };

    vec.x = vec1.x - vec2.x;
    vec.y = vec1.y - vec2.y;
    return vec;
}