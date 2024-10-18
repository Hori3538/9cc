#include <stdlib.h>
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

Node* expr()
{
    Node* node = equality();

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

Node* primary()
{
    // 次のトークンが"("なら、"(" expr ")"のはず
    if(consume("("))
    {
        Node* node = expr();
        expect(")");
        return node;
    }

    // そうでなければ数値のはず
    return new_node_num(expect_number());
}