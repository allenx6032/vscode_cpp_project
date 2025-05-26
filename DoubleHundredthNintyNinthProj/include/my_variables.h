/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** variables
*/

#ifndef VARIABLES_H_
    #define VARIABLES_H_

    #include <stddef.h>
    #include <stdbool.h>

const bool item_use[] = {
    false, //NONE
    true, //HELMET
    true, //BOOTS
    true, //SWORD
    true, //SHIELD
    false, //CATNIP_QUEST
    false, //MONACO_QUEST
    true, //CATNIP_ITEM
    true, //MONACO_ITEM
};

const char *const path_cat_texture[] = {
    "assets/sprites/cats/White_cat_1.png",
    "assets/sprites/cats/White_cat_2.png",
    "assets/sprites/cats/Light_brown_cat_1.png",
    "assets/sprites/cats/Light_brown_cat_2.png",
    "assets/sprites/cats/Brown_cat_1.png",
    "assets/sprites/cats/Brown_cat_2.png",
    "assets/sprites/cats/Black_cat_1.png",
    "assets/sprites/cats/Black_cat_2.png",
    NULL};

const char *const path_background_speach[] = {
    "assets/fonts/back_ground_text.png",
    "assets/fonts/back_ground_text.png",
    "assets/fonts/back_ground_text.png",
    "assets/fonts/back_ground_text.png",
    "assets/fonts/back_ground_text.png",
    NULL};

const char *const path_map_texture[] = {
    "assets/maps/Fishing_village.png",
    "assets/maps/Fishing_village_path.png",
    "assets/maps/Main_house.png",
    "assets/maps/Main_house_path.png",
    "assets/maps/INN.png",
    "assets/maps/INN_map1_path.png",
    "assets/maps/bar.png",
    "assets/maps/bar_path.png",
    "assets/maps/Forest_town.png",
    "assets/maps/Forest_town_path.png",
    "assets/maps/crack_house.png",
    "assets/maps/crack_house_path_red.png",
    "assets/maps/crack_house.png",
    "assets/maps/crack_house_path_grey.png",
    "assets/maps/potions_shop.png",
    "assets/maps/potions_shop_path.png",
    "assets/maps/teleportation_room.png",
    "assets/maps/teleportation_room_path.png",
    "assets/maps/weapon_shop.png",
    "assets/maps/weapon_shop_path.png",
    "assets/maps/armor_shop.png",
    "assets/maps/armor_shop_path.png",
    "assets/maps/Swamp.png",
    "assets/maps/Swamp_path.png",
    NULL};

const char *HELP_CONST = "./my_rpg [-h]\n"
    "USAGE\n"
    "\t-h help\n"
    "\tesc for menu\n"
    "\n"
    "INFORMATIONS\n"
    "\tCreators:\n"
    "\t\tMikael Sourati\n"
    "\t\tDamian Gil\n"
    "\t\tBenjamin Buisson\n"
    "\t\tGuillaume Deplaine\n";

const char *const TEXT[] = {
    //Main start
    "Hi, I'm the narrator and the god of this place.\nYou can "
    "press space to continue the dialog.",
    "If you need to change the settings, save or quit\npress Esc.",
    "As I said I'm god therefor I chose that you can move\nusing ZQSD"
    " and the arrow keys\nand interract using E.",
    "You can use LShift to run and LCtrl to skip dialogs",
    "and C to meow",
    //Quest 1 start
    "Hi there! Some guy stole my wallet and now\nI can't buy my monaco... "
    ,
    "Can you get me one at the bar please ?\nI'll pay you back.",
    //Quest 1 re-ask
    "Where's my monaco ?\nGo get me one at the bar please\n"
    "I promise I'll pay you back",
    //Quest 1 get
    "Hi what can I get you ?",
    "A monaco please.",
    "Here you go\n(You got a monaco, you can find it in your inventory)",
    //Quest 1 done
    "Here is your monaco\n(You give the monaco)",
    "Thanks a lot ! I'll remember you friend.",
    //Quest 2 start
    "I still can't find my wallet.\nCan you please get me some catnip",
    "I really need some. That's the last thing I'll ask you."
    //Quest 2 re-ask
    "You have my catnip ?\nIf not got at the shop please",
    //Quest 2 get
    "Hi what do you want to buy ?",
    "Catnip please.",
    "Here you go\n(You got some catnip, you can find it in your inventory)",
    //Quest 2 done
    "Here is your catnip\n(You give the catnip)",
    "I just learned what happened to your brother, I'm so sorry.\n",
    "It's not much but keep the catnip, come back anytime, I'll be here for "
    "you",
    //Death
    "Taken over by the cuteness of the cats you loose\nany interest in "
    "going against them.",
    //LVL UP
    "LVL UP\nYou gained hp, strenth and defense.",
    "MAX_LVL_REACHED",
    NULL};

const int xp_min[] = {20, 20, 30, 30, 30, 40, 60, -100};
const int xp_max[] = {20, 30, 35, 40, 50, 65, 80, 100};

const int xp_needed[] = {0, 100, 115, 132, 152, 174, 201, 231, 266, 305, 351,
    404, 465, 535, 615, 707, 813, 935, 1076, 1237, 1423, 1636, 1882, 2164, 2489,
    2862, 3291, 3785, 4353, 5006, 5757, 6621, 7614, 8756, 10069, 11580, 13317,
    15315, 17612, 20254, 23292, 26786, 30804, 35424, 40738, 46849, 53876, 61958,
    71252, __INT_MAX__};

const unsigned int ennemi_strenth[] = {10, 10, 15, 20, 25, 30, 40};
const unsigned int ennemi_defense[] = {2, 3, 4, 5, 6, 7, 8};
const unsigned int ennemi_health[] = {10, 20, 30, 30, 40, 40, 50};

#endif /* !VARIABLES_H_ */
