#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "9cc.h"

Token* token;
char* user_input;

void error(char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    exit(1);
}

void error_at(char* loc, char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " "); // pos個の空白を出力
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");

    exit(1);
}

bool consume(char* op)
{
    if(token->kind != TK_RESERVED ||
       strlen(op) != token->len ||
       memcmp(token->str, op, token->len))
        return false;
    token = token->next;

    return true;
}

Token* consume_ident()
{
    if(token->kind != TK_IDENT)
        return NULL;
    Token* ident_token = token;
    token = token->next;

    return ident_token;
}

void expect(char* op)
{
    if(token->kind != TK_RESERVED ||
       strlen(op) != token->len ||
       memcmp(token->str, op, token->len))
        error_at(token->str, "'%s'ではありません", op);
    token = token->next;
}

int expect_number()
{
    if(token->kind != TK_NUM)
        error_at(token->str, "数ではありません");
    int val = token->val;
    token = token->next;

    return val;
}

bool at_eof()
{
    return token->kind == TK_EOF;
}

Token* new_token(TokenKind kind, Token* cur, char* str)
{
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->next = NULL;
    tok->str = str;
    cur->next = tok;

    return tok;
}

Token* tokenize(char* p)
{
    Token head;
    head.next = NULL;
    Token* cur = &head;

    while(*p)
    {
        if(isspace(*p))
        {
            p++;
            continue;
        }

        // 二文字記号のトークナイズ
        if(strncmp(p, "==", 2)==0 || strncmp(p, "!=", 2)==0 || strncmp(p, "<=", 2)==0 || strncmp(p, ">=", 2)==0)
        {
            cur = new_token(TK_RESERVED, cur, p);
            cur->len = 2;
            p += 2;
            continue;
        }

        // 一文字記号のトークナイズ
        if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || 
           *p == '(' || *p == ')' || *p == '<' || *p == '>' ||
           *p == '=' || *p == ';')
        {
            cur = new_token(TK_RESERVED, cur, p++);
            cur->len = 1;
            continue;
        }

        // 数字のトークナイズ
        if(isdigit(*p))
        {
            cur = new_token(TK_NUM, cur, p);
            cur->len = 1;
            cur->val = strtol(p, &p, 10);
            continue;
        }

        // 一文字変数のトークナイズ
        if('a' <= *p && *p <= 'z')
        {
            cur = new_token(TK_IDENT, cur, p++);
            cur->len = 1;
            continue;
        }

        error_at(token->str, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p);
    cur->len = 1;

    return head.next;
}