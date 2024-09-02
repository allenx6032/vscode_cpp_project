#ifndef _10CC_H_
#define _10CC_H_

#define _GNU_SOURCE
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// parse.c
typedef struct LVar LVar;
struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
};

extern LVar *locals;

typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_NUM, // integer
  ND_EQ,
  ND_NE,
  ND_LT,
  ND_LE,
  ND_ASSIGN, // =
  ND_LVAR, // local variable
  ND_RETURN, // return
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
  LVar *var;
};

Node *expr();
void program();
extern Node *code[100];

// token.c
typedef enum {
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを表すトークン
  TK_RETURN,   // return
  TK_IF,       // if
  TK_ELSE      // else
} TokenKind;

typedef struct Token Token;

struct Token {
  int kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合、その数値
  char *str;      // トークン文字列
  int len;        // トークンの長さ
};

Token *tokenize(char *p);
void error_at(char *loc, char *fmt, ...);

// codegen.c
void gen(Node *node);

// util.c
void error(char *fmt, ...);


static char *strndup_(const char *s, size_t n) {
    char *result = malloc(n + 1); // 分配 n+1 个字节的空间
    if (result == NULL) {
        return NULL; // 内存分配失败
    }
    strncpy(result, s, n); // 复制 n 个字符
    result[n] = '\0'; // 确保字符串以空字符结束
    return result;
}

#endif
