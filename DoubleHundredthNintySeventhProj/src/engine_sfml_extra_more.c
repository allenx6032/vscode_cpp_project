/*
** EPITECH PROJECT, 2021
** sfml_extra_more
** File description:
** Allow even more extra sfml things
*/

#include "engine_rpg.h"
#include <math.h>

bool sfsprite_isspriteinside(const sfSprite *sprite1,
    const sfSprite *sprite2)
{
    const sfFloatRect sprite1_frec = sfSprite_getGlobalBounds(sprite1);
    const sfFloatRect sprite2_frec = sfSprite_getGlobalBounds(sprite2);

    if (sfFloatRect_intersects(
        &sprite1_frec,
        &sprite2_frec,
        NULL)){
            return sfTrue;
    }
    return sfFalse;
}

bool sfsprite_isspriteinside_permissive(const sfSprite *sprite1,
    const sfSprite *sprite2)
{
    sfFloatRect sprite1_frec = sfSprite_getGlobalBounds(sprite1);
    sfFloatRect sprite2_frec = sfSprite_getGlobalBounds(sprite2);

    sprite1_frec.height -= sprite2_frec.height * 2;
    sprite1_frec.width -= sprite2_frec.width * 2;
    sprite1_frec.top += sprite2_frec.height;
    sprite1_frec.left += sprite2_frec.width;
    if (sfFloatRect_intersects(
        &sprite1_frec,
        &sprite2_frec,
        NULL)){
            return sfTrue;
    }
    return sfFalse;
}

float sfvector2f_getdistance(const sfVector2f pos1,
    const sfVector2f pos2)
{
    float distx = (pos1.x - pos2.x)
        * (pos1.x - pos2.x);
    float disty = (pos1.y - pos2.y)
        * (pos1.y - pos2.y);
    return sqrt(distx + disty);
}

void sfsprite_setrotationself(sfSprite *sprite, const float angle)
{
    sfVector2f center = (sfVector2f) {
        sfSprite_getLocalBounds(sprite).width / 2,
        sfSprite_getLocalBounds(sprite).height / 2
    };

    sfSprite_setOrigin(sprite, center);
    sfSprite_setRotation(sprite, angle);
}