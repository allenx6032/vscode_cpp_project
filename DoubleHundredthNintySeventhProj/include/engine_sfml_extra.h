/*
** EPITECH PROJECT, 2021
** sfml_extra
** File description:
** Header of SFML Extra
*/

#ifndef SFML_EXTRA_H_
#define SFML_EXTRA_H_

#include <SFML_Graphics.h>
#include <stdbool.h>

/*    SFML EXTRA    */
bool sfvector2f_isdifferent(const sfVector2f pos1, const sfVector2f pos2);
sfVector2f sfvector2f_fixmousepositionwindow(const sfRenderWindow *window,
    const sfVector2f pos);
sfVector2f sfvector2f_fixmousepositionview(const sfRenderWindow *window,
    const sfView *view, const sfVector2f pos);
bool sfsprite_isvectorinside(const sfSprite *sprite, sfVector2f pos);
sfVector2f sfvector2f_diff(const sfVector2f vec1, const sfVector2f vec2);

/*    SFML EXTRA MORE    */
bool sfsprite_isspriteinside(const sfSprite *sprite1,
    const sfSprite *sprite2);
// Not sure, edited from isoutside but look like working good so far
bool sfsprite_isspriteinside_permissive(const sfSprite *sprite1,
    const sfSprite *sprite2);
float sfvector2f_getdistance(const sfVector2f pos1,
    const sfVector2f pos2);
void sfsprite_setrotationself(sfSprite *sprite, const float angle);

#endif /* !SFML_EXTRA_H_ */