/*
** EPITECH PROJECT, 2021
** map
** File description:
** Map basic game calls
*/

#include "engine_rpg.h"
#include "engine_teklibc.h"
#include "engine_sfml_extra.h"
#include "engine_music.h"
#include "engine_map.h"

bool map_sprite_can_move_at(map_data_t *current_map, sfSprite *moving_sprite,
    sfVector2f move_pos)
{
    sfSprite *simulated_sprite = moving_sprite;

    sfSprite_setPosition(simulated_sprite, move_pos);
    if (!sfsprite_isspriteinside_permissive(current_map->sprite,
        simulated_sprite))
        return false;
    if (map_is_sprite_inside_entity(current_map, simulated_sprite))
        return false;
    if (map_is_sprite_inside_building(current_map, simulated_sprite))
        return false;
    return true;
}

void map_init(game_t *game)
{
    game->current_map.sprite = sfSprite_create();
    game->current_map.texture =
        sfTexture_createFromFile("./data/map.png", NULL);
    sfSprite_setTexture(game->current_map.sprite,
        game->current_map.texture, sfFalse);
    sfSprite_setPosition(game->current_map.sprite, (sfVector2f){ 0, 0 });
    game->current_map.type = TEST_MAP;
    game->current_map.name = "Test Map";
    music_play(&game->musics, CHILL_MUSIC, true,
        (float) my_atoi(config_get(game->config, "volume")));
    entity_init(&game->current_map.world_entity);
    building_init(&game->current_map.world_building);
    particule_init(&game->current_map.world_particule);
    fill_map(game);
}

void map_update(game_t *game)
{
    entity_update(&game->current_map.world_entity, game);
    building_update(game, &game->current_map.world_building);
    particule_update(&game->current_map.world_particule);
}

void map_render(game_t *game)
{
    sfRenderWindow_drawSprite(game->window,
        game->current_map.sprite, NULL);
    entity_render(game->window, &game->current_map.world_entity);
    building_render(game->window, &game->current_map.world_building);
    particule_render(game->window, &game->current_map.world_particule);
}

void map_destroy(game_t *game)
{
    sfSprite_destroy(game->current_map.sprite);
    sfTexture_destroy(game->current_map.texture);
    entity_destroy(&game->current_map.world_entity);
    building_destroy(&game->current_map.world_building);
    particule_destroy(&game->current_map.world_particule);
    music_stop_all(&game->musics);
}