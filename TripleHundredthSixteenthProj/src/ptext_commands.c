#define _BSD_SOURCE
#define _DEFAULT_SOURCE
#define _GNU_SOURCE

#include <ptext_commands.h>
#include <ctype.h>
#include <ptext_files.h>
#include <ptext_stdafx.h>
#include <stdio.h>
#include <stdlib.h>
#include <ptext_userfuncs.h>
#include <ptext_utils.h>

static ssize_t static_my_getline(char **lineptr, size_t *n, FILE *stream) {
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

commandBind commands[] = {{"set-status", &user_setStatus},
                          {"set", &user_set},
                          {"goto-buffer", &user_gotoBuffer},
                          {"edit", &user_edit},
                          {NULL, NULL}};
char *getFunctionName(char *command, int *len);

void doLine(char *line) {
  int i;
  line = rtrim(line);
  int len;
  char *fname = getFunctionName(line, &len);
  if (fname == NULL) {
    return;
  }
  for (i = 0; commands[i].name != NULL; i++) {
    if (strcmp(commands[i].name, fname) == 0) {
      break;
    }
  }

  if (commands[i].name == NULL) {
    setStatusMessage("Function %s doesn't exist", fname);
    return;
  }

  args = &fname[len + 1];
  (void)(*commands[i].func)();
}

void execCommand(void) {
  char *command = getPrompt(":%s", NULL);
  if (command == NULL) {
    return;
  }
  doLine(command);
  free(command);
}

void doFile(char *filename) {
  if (access(filename, F_OK) == -1) {
    return;
  }
  FILE *fp = fopen(filename, "r");
  char *line = NULL;
  int len = 0;
  size_t size = 0;
  while ((len = (int)static_my_getline(&line, &size, fp)) != -1) {
    doLine(line);
  }
  fclose(fp);
  free(line);
}

char *getFunctionName(char *command, int *len) {
  command = &command[0];
  char *fname = token(command, len);
  return fname;
}
