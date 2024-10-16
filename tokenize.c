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

Token* new_token(TokenKind kind, Token* cur, char* str, int len)
{
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->next = NULL;
    tok->str = str;
    tok->len = len;
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
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        // 一文字記号のトークナイズ
        if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '<' || *p == '>')
        {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if(isdigit(*p))
        {
            // printf("row p: %c\n", *p);
            cur = new_token(TK_NUM, cur, p, 1);
            cur->val = strtol(p, &p, 10);
            // printf("p: %d\n", cur->val);
            continue;
        }

        error_at(token->str, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p, 1);
    return head.next;
}