/*
** EPITECH PROJECT, 2021
** lib_info
** File description:
** An easy way to recover informations from lib
*/

#include "engine_teklibc.h"

void my_printlibinfo(void)
{
    my_putstr("****** LIB INFO ******\n");
    my_putstr("VERSION = ");
    my_putint(LIB_MAJOR_VERSION);
    my_putchar('.');
    my_putint(LIB_MINOR_VERSION);
    my_putchar('\n');
    my_putstr("**** END LIB INFO ****\n");
}