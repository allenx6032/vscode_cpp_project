/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** my
*/

#ifndef MY_H_
    #define MY_H_

    #include "my_typedef.h"

bool save(char_info_t *perso, char **env);
void moving_cases(rpg_t *rpg);
void play_meow(rpg_t *rpg);
void focus_test(rpg_t *rpg);
void map_move_monaco(sfVector2f monaco_move, rpg_t *rpg);
void serve_monaco(rpg_t *rpg);
void update_map_position(rpg_t *rpg, enum map_names map);
void moves_dir(rpg_t *rpg, sfIntRect rect, sfVector2f pos,
    sfVector2f pixel_pos);
void text_box(rpg_t *rpg, unsigned int text_num, enum speach_box who_speak);
void print_text(rpg_t *rpg, enum text start, enum text end,
    enum speach_box who_speak);
void misc_events(rpg_t *rpg);
void keys_pressed(rpg_t *rpg, char **env);
void loop(rpg_t *rpg, char **env);
void ennemy_attacking(rpg_t *rpg);
void main_music(rpg_t *rpg);
char_info_t *set_perso(char_info_t *perso);
char_info_t *attribute_perso(char_info_t *perso);
void catnip(rpg_t *rpg);

//Fight
void check_collision(rpg_t *rpg, enum cat_names name);
void set_circle(rpg_t *rpg, enum cat_names name);
void move_ennemi(rpg_t *rpg, enum cat_names name);
void death(rpg_t *rpg);
void ennemy_attack(rpg_t *rpg, enum cat_names name);
void play_claw_music(void);
void third_if(rpg_t *rpg);
void perso_attack(rpg_t *rpg);
void what_if_ennemy(rpg_t *rpg, sfVector2f movement, enum cat_names name);
void ennemy_attack2(rpg_t *rpg, float dist, sfClock* last_attack_clock_perso,
    enum cat_names name);
void add_xp(rpg_t *rpg, enum cat_names name);
void for_ennemy_attack2(rpg_t *rpg, enum cat_names name);

//Collisions
void cases_red_coll(rpg_t *rpg);
void cases_green_coll(rpg_t *rpg);
void cases_yellow_coll(rpg_t *rpg);
void cases_pink_coll(rpg_t *rpg);
void cases_purple_coll(rpg_t *rpg);
void cases_grey_coll(rpg_t *rpg);
void cases_blue_coll(rpg_t *rpg);
void cases_orange_coll(rpg_t *rpg);
void green_switch(rpg_t *rpg);
void yellow_switch(rpg_t *rpg);
void red_collision(rpg_t *rpg);
void green_collision(rpg_t *rpg);
void yellow_collision(rpg_t *rpg);
void purple_collision(rpg_t *rpg);
void blue_collision(rpg_t *rpg);
void draw_health(rpg_t *rpg);
void pink_collision(rpg_t *rpg);
void grey_collision(rpg_t *rpg);
void orange_collision(rpg_t *rpg);
bool check_red(rpg_t *rpg, sfVector2f pixel_pos);
bool check_pink(rpg_t *rpg, sfVector2f pixel_pos);
bool check_green(rpg_t *rpg, sfVector2f pixel_pos);
bool check_yellow(rpg_t *rpg, sfVector2f pixel_pos);
bool check_blue(rpg_t *rpg, sfVector2f pixel_pos);
bool check_purple(rpg_t *rpg, sfVector2f pixel_pos);
bool check_grey(rpg_t *rpg, sfVector2f pixel_pos);
bool check_orange(rpg_t *rpg, sfVector2f pixel_pos);
void case_pink(rpg_t *rpg, sfVector2f pixel_pos, sfIntRect rect);
void case_green(rpg_t *rpg, sfVector2f pixel_pos, sfIntRect rect);
void case_red(rpg_t *rpg, sfVector2f pixel_pos, sfIntRect rect);
void case_yellow(rpg_t *rpg, sfVector2f pixel_pos, sfIntRect rect);
void case_blue(rpg_t *rpg, sfVector2f pixel_pos, sfIntRect rect);
void case_purple(rpg_t *rpg, sfVector2f pixel_pos, sfIntRect rect);
void case_grey(rpg_t *rpg, sfVector2f pixel_pos, sfIntRect rect);
void case_orange(rpg_t *rpg, sfVector2f pixel_pos, sfIntRect rect);
void id_switch(rpg_t *rpg);

//Quests
void quest_1(rpg_t *rpg);

#endif /* !MY_H_ */
