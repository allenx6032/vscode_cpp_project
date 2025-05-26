/*
** EPITECH PROJECT, 2021
** file_basic
** File description:
** Read a file was difficult in the past... the PAST !
*/

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

char *my_fread(const char *path)
{
    char *buff = NULL;
    struct stat *em = malloc(sizeof(*em));
    int fd = open(path, O_RDONLY);

    if (path == NULL)
        return NULL;
    if (stat(path, em) != 0)
        return NULL;
    buff = malloc(sizeof(char) * (em->st_size + 1));
    if (buff == NULL)
        return NULL;
    if (read(fd, buff, em->st_size) == -1)
        return NULL;
    buff[em->st_size] = '\0';
    free(em);
    return buff;
}