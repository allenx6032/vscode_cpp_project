/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** typedef
*/

#ifndef TYPEDEF_H_
    #define TYPEDEF_H_

    #include <SFML_Audio.h>
    #include <SFML_Graphics.h>
    #include <stdbool.h>

    enum text {
        MAIN_START = 0,
        MAIN_END = MAIN_START + 4,
        QUEST_1_UNKNOWN_START = MAIN_END + 1,
        QUEST_1_UNKNOWN_END = QUEST_1_UNKNOWN_START + 1,
        QUEST_1_KNOWN_START = QUEST_1_UNKNOWN_END + 1,
        QUEST_1_KNOWN_END = QUEST_1_KNOWN_START,
        QUEST_1_GET_START = QUEST_1_KNOWN_END + 1,
        QUEST_1_GET_END = QUEST_1_GET_START + 2,
        QUEST_1_DONE_START = QUEST_1_GET_END + 1,
        QUEST_1_DONE_END = QUEST_1_DONE_START + 1,
        QUEST_2_UNKNOWN_START = QUEST_1_DONE_END + 1,
        QUEST_2_UNKNOWN_END = QUEST_2_UNKNOWN_START,
        QUEST_2_KNOWN_START = QUEST_2_UNKNOWN_END + 1,
        QUEST_2_KNOWN_END = QUEST_2_KNOWN_START,
        QUEST_2_GET_START = QUEST_2_KNOWN_END + 1,
        QUEST_2_GET_END = QUEST_2_GET_START + 2,
        QUEST_2_DONE_START = QUEST_2_GET_END + 1,
        QUEST_2_DONE_END = QUEST_2_DONE_START + 2,
        DEATH_START = QUEST_2_DONE_END + 1,
        DEATH_END = DEATH_START,
        LVL_UP_START = DEATH_END + 1,
        LVL_UP_END = LVL_UP_START,
        LVL_MAX_START = LVL_UP_END + 1,
        LVL_MAX_END = LVL_MAX_START
    };

    enum speach_box {
        NOONE,
        BENJ,
        DAMIAN,
        MIKAEL,
        GUILL
    };

    enum quest_order {
        QUEST_1,
        QUEST_2
    };

    enum map_names {
        MAP_1,
        MAP_1_PATH,
        HOUSE1,
        HOUSE1_PATH,
        INN,
        INN_PATH,
        BAR,
        BAR_PATH,
        MAP_2,
        MAP_2_PATH,
        CRACK_HOUSE_RED,
        CRACK_HOUSE_RED_PATH,
        CRACK_HOUSE_GRAY,
        CRACK_HOUSE_GRAY_PATH,
        POTION,
        POTION_PATH,
        TP_ROOM,
        TP_ROOM_PATH,
        WEAPON,
        WEAPON_PATH,
        ARMOR,
        ARMOR_PATH,
        SWAMP,
        SWAMP_PATH
    };

    enum quest_status {
        UNKNOWN,
        KNOWN,
        DONE,
        ENDED
    };

    enum cat_names {
        WHITE_CAT,
        WHITE_2_CAT,
        LIGHT_BROWN_CAT,
        LIGHT_BROWN_2_CAT,
        BROWN_CAT,
        BROWN_2_CAT,
        BLACK_CAT,
        BLACK_2_CAT,
        PLAYER = WHITE_2_CAT
    };

    enum xp_order {
        XP_WHITE_CAT = WHITE_CAT,
        XP_WHITE_2_CAT = WHITE_2_CAT,
        XP_LIGHT_BROWN_CAT = LIGHT_BROWN_CAT,
        XP_LIGHT_BROWN_2_CAT = LIGHT_BROWN_2_CAT,
        XP_BROWN_CAT = BROWN_CAT,
        XP_BROWN_2_CAT = BROWN_2_CAT,
        XP_BLACK_CAT = BLACK_CAT,
        XP_BLACK_2_CAT = BLACK_2_CAT,
        CASINO
    };

    typedef struct cats {
        sfSprite **sprites;
        sfTexture **textures;
        sfCircleShape **hitbox;
        int *health;
        bool *is_alive;
    } cats_t;

    typedef struct statistics {
        unsigned int xp;
        unsigned int lvl;
        unsigned int hp_max;
        unsigned int xp_to_next_level;
        int strenth;
        int defense;
        int hp;
    } statistics_t;

typedef struct items items_t;

    typedef struct character_informations {
        items_t *inventory;
        size_t inventory_size;
        unsigned int pos_x;
        unsigned int pos_y;
        float map_pos_x;
        float map_pos_y;
        unsigned int id_zone;
        bool is_ennemi_attacking;
        bool is_attacking;
        unsigned int old_zone;
        enum quest_status *quest;
        statistics_t *stats;
    } char_info_t;

    typedef struct character {
        sfSprite *sprite;
        sfMusic *music;
        sfSprite *sprite_claw;
        sfSprite *sprite_red_claw;
        sfTexture *texture;
        sfTexture *texture_claw;
        sfTexture *texture_red_claw;
        cats_t *cats;
        char_info_t *perso;
        sfCircleShape *hitbox;
        sfClock *clock;
        sfClock *clock2;
        sfClock *clock3;
        sfSoundBuffer *cat_sound;
        sfSound *meow;
        sfInt32 speed;
        sfRectangleShape *health;
        sfRectangleShape *out_health;
        sfSprite *monaco_spr;
        sfTexture *monaco_tex;
    } character_t;

typedef struct my_ennemi {
        sfVector2f pos;
        sfVector2f map_pos;
        sfFloatRect size_char;
        sfVector2f pixel_pos;
    } ennemi_t;

    typedef struct map {
        sfSprite **sprite;
        sfTexture **texture;
        sfImage **image;
    } map_t;

    typedef struct menu {
        struct map *map;
    } menu_t;

    typedef struct rpg {
        sfRenderWindow *window;
        sfEvent event;
        struct character *character;
        struct menu *menu;
        char **env;
    } rpg_t;

#endif
