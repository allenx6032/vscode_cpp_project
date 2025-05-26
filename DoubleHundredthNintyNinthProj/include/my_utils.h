/*
** EPITECH PROJECT, 2023
** B-MUL-200-PAR-2-1-myrpg-damian.gil
** File description:
** utils
*/

#ifndef UTILS_H_
    #define UTILS_H_

    #include <stddef.h>
    #include <stdbool.h>

void *malloc_empty(size_t size);
int my_strlen(char const *str);
int my_strncmp(const char* s1, const char* s2, int n);
char *my_strdup(char const *src);
char *my_strcpy(char *dest, char const *src);
size_t my_array_len(void *ptr);
char *my_strcat(char *dest, const char *src);
char *my_strnbr_base(int nbr, char const *base);
bool set_rand(void);
int get_rand(void);
int get_randint(int min, int max);

#endif
