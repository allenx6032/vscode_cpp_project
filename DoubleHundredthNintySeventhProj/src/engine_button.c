/*
** EPITECH PROJECT, 2021
** button
** File description:
** button
*/

#include <stdlib.h>
#include "engine_rpg.h"
#include "engine_font.h"
#include "engine_button.h"

void button_init(game_t *game)
{
    my_vector_init(&game->buttons);
}

void button_create(game_t *game, button_type type, sfVector2f position)
{
    button_t *button = malloc(sizeof(button_t));
    sfIntRect rec = (sfIntRect){ 0, 0, (188 * 1), 48};

    button->type = type;
    button->state = IDLE_BUTTON_STATE;
    button->position = position;
    button->sprite = sfSprite_create();
    button->texture = sfTexture_createFromFile("./data/buttons.png", NULL);
    sfTexture_setSmooth(button->texture, sfTrue);
    button->text = sfText_create();
    sfText_setFont(button->text, font_get(&game->fonts,
        ANCIENT_MEDIUM_FONT)->font);
    sfText_setString(button->text,
        button_str_from_type(game->config, button->type));
    sfText_setPosition(button->text, button->position);
    sfSprite_setTexture(button->sprite, button->texture, sfTrue);
    sfText_setCharacterSize(button->text, 24);
    sfSprite_setTextureRect(button->sprite, rec);
    sfSprite_setPosition(button->sprite, button->position);
    my_vector_add(&game->buttons, button);
}

void button_update(game_t *game)
{
    button_t *button;
    sfIntRect rec;

    for (int i = 0; i != game->buttons.size; i++) {
        button = (button_t *) my_vector_get(&game->buttons, i);
        button_update_state(game, button);
        rec = (sfIntRect){  (188 * (button->state)), 0, 188, 48};
        sfText_setString(button->text,
            button_str_from_type(game->config, button->type));
        sfText_setColor(button->text, sfColor_fromRGB(242 - (button->state * 50)
            , 243 - (button->state * 50), 244 - (button->state * 60)));
        sfSprite_setTextureRect(button->sprite, rec);
        sfSprite_setPosition(button->sprite, button->position);
        sfFloatRect text_rect = sfText_getLocalBounds(button->text);
        sfText_setOrigin(button->text, (sfVector2f){
            (text_rect.left + (text_rect.width / 2.0f)),
            (text_rect.top + (text_rect.height / 2.0f))});
        sfText_setPosition(button->text, (sfVector2f){
            button->position.x + (rec.width / 2.0f),
            button->position.y + (rec.height / 2.0f)});
    }
}

void button_render(game_t *game)
{
    button_t *button;

    for (int i = 0; i != game->buttons.size; i++) {
        button = (button_t *) my_vector_get(&game->buttons, i);
        sfRenderWindow_drawSprite(game->window, button->sprite, NULL);
        sfRenderWindow_drawText(game->window, button->text, NULL);
    }
}

void button_destroy(game_t *game)
{
    button_t *result;

    for (int i = 0; i != game->buttons.size; i++) {
        result = (button_t *) my_vector_get(&game->buttons, i);
        sfSprite_destroy(result->sprite);
        sfTexture_destroy(result->texture);
        sfText_destroy(result->text);
        free(result);
    }
    my_vector_free(&game->buttons);
}