/*
** EPITECH PROJECT, 2023
** my_putchar
** File description:
** ere
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "my.h"

void my_putchar(char c)
{
    write(1, &c, 1);
}

char *my_strncat(char *dest, char *src, int nb)
{
    int len_dest = my_strlen(dest);
    int len_src = my_strlen(src);
    char a;
    int j = 0;
    int i = 0;

    if (len_src < nb) {
        for (j; j < len_src; j++) {
            dest[len_dest + j] = src[j];
        }
        return (dest);
    }
    for (; i < nb; i++) {
        if (i < nb) {
            a = src[i];
            dest[len_dest + i] = a;
        }
    }
    dest[len_dest] = '\0';
    return (dest);
}

char *my_strcat(char *dest, char *src)
{
    int i = 0;
    int len_dest = 0;
    int len_src = 0;
    char *new = NULL;

    if (dest == NULL || src == NULL)
        return NULL;
    len_src = my_strlen(src);
    len_dest = my_strlen(dest);
    new = malloc(sizeof(char) * (len_dest + len_src + 1));
    my_strcpy(new, dest);
    for (i; i < len_src; i++)
            new[len_dest + i] = src[i];
    new[len_dest + i] = '\0';
    return (new);
}
// int main(void)
// {
//     printf("%s\n", my_strcat("hello", "hi"));
//     return 0;
// }
