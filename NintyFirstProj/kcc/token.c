#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "kcc.h"

static Vector *tokens;

static Token *add_token(int ty, char *intput) {
    Token *token = calloc(1, sizeof(Token));
    token->ty = ty;
    token->input = intput;
    vec_push(tokens, token);
    return token;
}

static struct {
    char *name;
    int ty;
} symbols[] = {
    {"==", TK_EQ}, {"!=", TK_NE}, {"&&", TK_LOGAND}, {"||", TK_LOGOR},
};

static Map *keywords;

static Map *keyword_map() {
    Map *map = new_map();
    map_puti(map, "if", TK_IF);
    map_puti(map, "else", TK_ELSE);
    map_puti(map, "for", TK_FOR);
    map_puti(map, "do", TK_DO);
    map_puti(map, "while", TK_WHILE);
    map_puti(map, "return", TK_RETURN);
    map_puti(map, "sizeof", TK_SIZEOF);
    map_puti(map, "extern", TK_EXTERN);
    map_puti(map, "int", TK_INT);
    map_puti(map, "char", TK_CHAR);
    return map;
}

char *_strndup(const char *s, size_t n) {
    char *result = malloc(n + 1); // 分配 n+1 个字节的空间
    if (result == NULL) {
        return NULL; // 内存分配失败
    }
    strncpy(result, s, n); // 复制 n 个字符
    result[n] = '\0'; // 确保字符串以空字符结束
    return result;
}


static char *string_literal(char *p) {
    assert(*p == '"');
    Token *t = add_token(TK_STR, p++);
    StringBuilder *sb = new_sb();
    for (;*p != '"';p++) {
        if (!*p)
            error("string literal is broken");
        if (*p == '\\') {
            p++;
            // escape sequence
            static char escaped_table[256] = {
                ['a'] = '\a', ['b'] = '\b', ['f'] = '\f', ['n'] = '\n',
                ['r'] = '\r', ['t'] = '\t', ['v'] = '\v',
            };
            if (escaped_table[(int)*p]) {
                sb_add(sb, escaped_table[(int)*p]);
                continue;
            }
        }
        // TODO: escape sequence
        sb_add(sb, *p);
    }
    p++;
    t->data = sb_string(sb);
    return p;
}

// parse p to tokens
Vector *tokenize(char *p) {
    keywords = keyword_map();
    tokens = new_vector();

loop:
    while (*p) {
        // skip whitespace
        if (isspace(*p)) {
            p++;
            continue;
        }

        for (int i = 0; symbols[i].name; i++) {
            char *name = symbols[i].name;
            int len = strlen(name);
            if (strncmp(p, name, len) == 0) {
                add_token(symbols[i].ty, p);
                p += len;
                goto loop;
            }
        }

        if (isalpha(*p) || *p == '_') {
            int len = 1;
            while (isalpha(p[len]) || p[len] == '_' || isdigit(p[len])) {
                len++;
            }
            // char *name = strndup(p, len);
            char *name = _strndup(p, len);
            if (map_exists(keywords, name)) {
                int ty = map_geti(keywords, name);
                add_token(ty, p);
                p+=len;
                continue;
            }

            Token *t =add_token(TK_IDENT, p);
            t->name = name;
            p+=len;
            continue;
        }

        if (strchr("+-*/()=;,{}&[]", *p)) {
            add_token(*p, p);
            p++;
            continue;
        }

        if (isdigit(*p)) {
            Token *token = add_token(TK_NUM, p);
            token->val = strtol(p, &p, 10);
            continue;
        }

        // string literal
        if (*p == '"') {
            p = string_literal(p);
            continue;
        }

        error("can not tokenize: %s\n", p);
    }

    add_token(TK_EOF, p);
    return tokens;
}
