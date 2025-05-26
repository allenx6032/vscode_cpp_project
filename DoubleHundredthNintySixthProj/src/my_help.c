/*
** EPITECH PROJECT, 2023
** MUL_my_hunter_
** File description:
** help.c
*/
#include "my.h"

void output_help(void)
{
    my_putstr("\nusage: ./my_hunter [option]\n");
    my_putstr("options: -h\n");
    my_putstr("\n  ");
    my_putstr("This small video game is based on the rules of Duck Hunt.\n\n");
    my_putstr("  The basic rules for the my_hunter are as follows: \n");
    my_putstr("    • the player is a hunter who shoots ducks.\n");
    my_putstr("    • ducks must appear on the screen ");
    my_putstr("and move from one side to another.\n");
    my_putstr("    • the player can click on them to shoot them.\n\n");
}
