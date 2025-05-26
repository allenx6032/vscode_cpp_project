/*
** EPITECH PROJECT, 2021
** teklibc
** File description:
** Header for V.M. Tek Lib C
*/

/*
    Include Picker :D

    #include <stdlib.h> -> malloc...
    #include <unistd.h> -> NULL, write...
    #include <stdarg.h> -> valist, flag...

    Lib Edited for RPG :
        Vector is in another struct
        Removed fonctions that use not allowed fonctions

    Change Logs :

    3.1 :   Updated get_line() : fixed free()

    3.2 :   New Makefile + Exclusive TU Makefile
            arg_exist fixed invalid read when ac <= 1

    3.3 :   Added my_strcontains

    3.4 :   Added can_exec, have_right, file_exist,
            is_dir & is_file

    3.5 :   Added my_strtok and my_strtok_r
*/

#ifndef TEKLIBC_H_
#define TEKLIBC_H_

#include <stddef.h>
#include <stdbool.h>

#define LIB_MAJOR_VERSION (int) 3
#define LIB_MINOR_VERSION (int) 5

#define GET_LINE_BUFFER (int) 512

void my_putchar(const char c);
char my_char_uper(char c);
char my_char_lower(char c);

int my_strlen(const char *str);
void my_putstr(const char *str);
char *my_strrev(char *str);
char *my_str_uper(char *str);
char *my_str_lower(char *str);

char *my_strdup(const char *src);
int my_strcmp(const char *str1, const char *str2);
bool my_str_is_null_or_empty(const char *str);
bool my_strcontains(const char *input, const char *check);
char *my_strcat(const char *init, const char *to_add);

char *my_strtok(char *str, char delim);
char *my_strtok_r(char *str, char delim, char **saveptr);

void my_putint(const int nb);
int my_intlen(int nb);
int my_atoi(const char *str);
char *my_itoa(int nb);
void my_putint(const int nb);

bool arg_exist(int ac, char **av, char *arg);

char *my_fread(const char *path);

char *my_getline(void);

void my_printlibinfo(void);

#endif /* !TEKLIBC_H_ */
