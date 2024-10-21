#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

Node* new_node(NodeKind kind, Node* lhs, Node* rhs)
{
    Node* node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;

    return node;
}

Node* new_node_num(int val)
{
    Node* node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;

    return node;
}

LVar* locals;

LVar* find_lvar(Token* token)
{
    for(LVar* var = locals; var; var = var->next)
        if(var->len == token->len && !memcmp(token->str, var->name, var->len))
            return var;
    
    return NULL;
}

Node* code[100];

Node* program()
{
    int i = 0;
    while (!at_eof())
        code[i++] = stmt();
    code[i] = NULL;    
}

Node* stmt()
{
    Node* node;

    if(consume_return(TK_RETURN))
    {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
    }
    else
    {
        node = expr();
    }
    expect(";");

    return node;
}

Node* expr()
{
    Node* node = assign();

    return node;
}

Node* assign()
{
    Node* node = equality();

    if(consume("="))
        node = new_node(ND_ASSIGN, node, assign());
    
    return node;
}

Node* equality()
{
    Node* node = relational();

    for(;;)
    {
        if(consume("=="))
            node = new_node(ND_EQUAL, node, relational());
        else if(consume("!="))
            node = new_node(ND_NOT_EQUAL, node, relational());
        else
            return node;
        
    }
}

Node* relational()
{
    Node* node = add();

    for(;;)
    {
        if(consume("<"))
            node = new_node(ND_LESS_THAN, node, add());
        else if(consume("<="))
            node = new_node(ND_LESS_THAN_EQUAL, node, add());
        else if(consume(">"))
            node = new_node(ND_MORE_THAN, node, add());
        else if(consume(">="))
            node = new_node(ND_MORE_THAN_EQUAL, node, add());
        else
            return node;
    }
}

Node* add()
{
    Node* node = mul();

    for(;;)
    {
        if(consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if(consume("-"))
            node = new_node(ND_SUB, node, mul());
        else 
            return node;
    }
}

Node* mul()
{
    Node* node = unary();

    for(;;)
    {
        if(consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if(consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

Node* unary()
{
    if(consume("+"))
        return primary();
    if(consume("-"))
        return new_node(ND_SUB, new_node_num(0), primary());
    
    return primary();
}

int local_count = 0;
Node* primary()
{
    // 次のトークンが"("なら、"(" expr ")"のはず
    if(consume("("))
    {
        Node* node = expr();
        expect(")");
        return node;
    }

    // 次のトークンが識別子ならND_LVAR型のノードを返す
    Token* token = consume_ident();
    if(token)
    {
        Node* node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;

        LVar* lvar = find_lvar(token);
        if(lvar)
        {
            node->offset = lvar->offset;
        }
        else
        {
            lvar = calloc(1, sizeof(LVar));
            lvar->next = locals;
            lvar->name = token->str;
            lvar->len = token->len;
            lvar->offset = locals->offset + 8;
            node->offset = lvar->offset;
            locals = lvar;
            local_count++;
        }

        return node;
    }

    // そうでなければ数値のはず
    return new_node_num(expect_number());
}