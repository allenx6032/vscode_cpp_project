/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** sprites
*/

#include "my.h"
#include "my_create.h"
#include "my_var.h"
#include "my_defines.h"

void setup_cats(cats_t *cats)
{
    for (unsigned int i = 0; i < CAT_NAMES_SIZE; i++) {
        cats->textures[i] = sfTexture_createFromFile(path_cat_texture[i], NULL);
        cats->sprites[i] = sfSprite_create();
        sfSprite_setTexture(cats->sprites[i], cats->textures[i], sfTrue);
        sfSprite_setScale(cats->sprites[i], (sfVector2f){3, 3});
        sfSprite_setTextureRect(cats->sprites[i], (sfIntRect){0, 0, 31, 32});
        sfSprite_setPosition(cats->sprites[i], (sfVector2f){1100, 900});
        cats->hitbox[i] = sfCircleShape_create();
        sfCircleShape_setOutlineColor(cats->hitbox[i], sfTransparent);
        sfCircleShape_setOutlineThickness(cats->hitbox[i], 1);
        sfCircleShape_setFillColor(cats->hitbox[i], sfTransparent);
    }
}

void setup_hitbox(character_t *character)
{
    character->hitbox = sfCircleShape_create();
    sfCircleShape_setRadius(character->hitbox, 50);
    sfCircleShape_setFillColor(character->hitbox, sfTransparent);
    sfCircleShape_setOutlineColor(character->hitbox, sfTransparent);
    sfCircleShape_setOutlineThickness(character->hitbox, 1);
    sfCircleShape_setPosition(character->hitbox,
        sfSprite_getPosition(character->sprite));
}

void setup_sprites(rpg_t *rpg)
{
    rpg->character->texture = sfTexture_createFromFile
        (path_cat_texture[PLAYER], NULL);
    rpg->character->sprite = sfSprite_create();
    sfSprite_setTexture(rpg->character->sprite,
        rpg->character->texture, sfTrue);
    sfSprite_setTextureRect(rpg->character->sprite, (sfIntRect){0, 0, 31, 32});
    sfSprite_setScale(rpg->character->sprite, (sfVector2f){3, 3});
    sfSprite_setPosition(rpg->character->sprite, (sfVector2f){550, 890});
    setup_hitbox(rpg->character);
    setup_maps(rpg);
    setup_cats(rpg->character->cats);
    setup_health(rpg);
    setup_monaco(rpg->character);
}

void setup_claw(rpg_t *rpg)
{
    rpg->character->texture_claw = sfTexture_createFromFile
        ("assets/sprites/Animations/Group 1.png", NULL);
    rpg->character->sprite_claw = sfSprite_create();
    sfFloatRect characterBounds = sfSprite_getGlobalBounds
        (rpg->character->cats->sprites[BLACK_2_CAT]);

    sfSprite_setTexture(rpg->character->sprite_claw, rpg->
        character->texture_claw, sfTrue);
    sfSprite_setTextureRect(rpg->character->sprite_claw,
        (sfIntRect){0, 0, 192, 192});
    sfSprite_setScale(rpg->character->sprite_claw,
        (sfVector2f){0.5, 0.5});
    sfSprite_setPosition(rpg->character->sprite_claw,
        (sfVector2f){characterBounds.left +
        characterBounds.width / 5.5f,
        characterBounds.top - sfSprite_getGlobalBounds
        (rpg->character->sprite_claw).height / 5.5f});
    setup_claw2(rpg);
}

void setup_claw2(rpg_t *rpg)
{
    rpg->character->texture_red_claw = sfTexture_createFromFile
    ("assets/sprites/Animations/red_claws.png", NULL);
    rpg->character->sprite_red_claw = sfSprite_create();
    sfFloatRect character_red_Bounds = sfSprite_getGlobalBounds
    (rpg->character->sprite);

    sfSprite_setTexture(rpg->character->sprite_red_claw,
    rpg->character->texture_red_claw, sfTrue);
    sfSprite_setTextureRect(rpg->character->sprite_red_claw,
    (sfIntRect){0, 0, 192, 192});
    sfSprite_setScale(rpg->character->sprite_red_claw,
    (sfVector2f){0.5, 0.5});
    sfSprite_setPosition(rpg->character->sprite_red_claw,
        (sfVector2f){character_red_Bounds.left +
        character_red_Bounds.width / 5.5f,
        character_red_Bounds.top - sfSprite_getGlobalBounds
        (rpg->character->sprite_red_claw).height / 5.5f});
}
