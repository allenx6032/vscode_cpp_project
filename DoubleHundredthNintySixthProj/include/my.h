/*
** EPITECH PROJECT, 2023
** my.h
** File description:
** wdfvb
*/

#pragma once
#include <stdbool.h>

void my_putchar(char c);
int my_strlen(char *str);
int my_put_nbr(long long int nb);
int my_put_nbr_rec(long long int nb, int a);
int my_putstr(char const *str);
char *my_strcpy(char *dest, char *src);
bool my_strcmp(char *s1, char *s2);
char *my_strdup(char *src);
char *my_strncat(char *dest, char *src, int nb);
char *my_strcat(char *dst, char *src);
char *tostr(long long int nb);
