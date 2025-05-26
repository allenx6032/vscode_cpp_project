/*
** EPITECH PROJECT, 2023
** libmy
** File description:
** return lenth of string
*/

int my_strlen(char const *str)
{
    int i = 0;

    while (str[i] != '\0') {
        i++;
    };
    return i;
}
