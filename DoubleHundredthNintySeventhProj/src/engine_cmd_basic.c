/*
** EPITECH PROJECT, 2021
** cmd_basic
** File description:
** Because recover a line should be easy
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "engine_teklibc.h"



#include <stdio.h>
#include <stdlib.h>
 
static ssize_t my_static_getline(char **lineptr, size_t *n, FILE *stream) {
    if (lineptr == NULL || n == NULL || stream == NULL) {
        return -1;  // 无效参数检查
    }
 
    size_t capacity = *n;  // 当前缓冲区大小
    size_t length = 0;     // 当前已读取的字符数
    int c;
 
    // 如果给定的缓冲区大小为 0 或指针为 NULL，则分配初始内存
    if (capacity == 0) {
        capacity = 128;  // 默认分配 128 字节的内存
        *lineptr = (char *)malloc(capacity);
        if (*lineptr == NULL) {
            return -1;  // 内存分配失败
        }
    }
 
    // 逐个字符读取直到换行符或者文件结束符
    while ((c = fgetc(stream)) != EOF) {
        // 如果当前字符是换行符，停止读取
        if (c == '\n') {
            break;
        }
 
        // 检查当前缓冲区是否足够存储读取的字符
        if (length + 1 >= capacity) {
            capacity *= 2;  // 扩展内存
            *lineptr = (char *)realloc(*lineptr, capacity);
            if (*lineptr == NULL) {
                return -1;  // 内存重新分配失败
            }
        }
 
        // 将当前字符存储到缓冲区
        (*lineptr)[length++] = (char)c;
    }
 
    // 如果没有读取到任何字符并且文件结束，则返回 -1
    if (length == 0 && c == EOF) {
        return -1;
    }
 
    // 添加字符串结束符
    (*lineptr)[length] = '\0';
 
    // 更新缓冲区大小
    *n = capacity;
    return length;
}

char *my_getline(void)
{
    size_t size = GET_LINE_BUFFER;
    char *str = (char *) malloc(size);
    int ctrd_check = -1;
    int len;

    ctrd_check = my_static_getline(&str, &size, stdin);
    if (ctrd_check == -1) {
        free(str);
        return NULL;
    }
    len = my_strlen(str);
    for (int i = len; i != -1; i--)
        if (str[i] == '\n')
            str[i] = '\0';
    return str;
}