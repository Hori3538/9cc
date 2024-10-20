#include <stdbool.h>

// トークンの種類
typedef enum
{
    TK_RESERVED, // 記号
    TK_IDENT, // 識別子
    TK_NUM, // 整数トークン
    TK_EOF, // 入力の終わり表すトークン
} TokenKind;

// トークン型
typedef struct Token Token;
struct Token
{
    TokenKind kind; // トークンの型
    Token* next; // 次の入力トークン
    int val; // kindがTK_NUMの場合、その数値
    char* str; // トークン文字列
    int len; // トークンの長さ
};

// 抽象構文木のノードの種類
typedef enum
{
    ND_EQUAL, // ==
    ND_NOT_EQUAL, // !=
    ND_LESS_THAN, // <
    ND_LESS_THAN_EQUAL, // <=
    ND_MORE_THAN, // >
    ND_MORE_THAN_EQUAL, // >=
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_NUM, // 整数
    ND_ASSIGN, // =
    ND_LVAR, // ローカル変数
} NodeKind;

typedef struct Node Node;
// 抽象構文木のノードの型
struct Node
{
    NodeKind kind; // ノードの型
    Node* lhs; // 左辺
    Node* rhs; // 右辺
    int val; // kindがND_NUMの場合のみ扱う
    int offset; // kindがND_LVARの場合のみ使う
};

typedef struct LVar LVar;
// ローカル変数の型
struct LVar
{
    LVar* next; // 次の変数かNULL
    char* name; //変数の名前
    int len; // 名前の長さ
    int offset; // RBPからのオフセット
};

// 現在着目しているトークン
extern Token* token;
// 入力プログラム
extern char* user_input;
extern Node* code[100];
// ローカル変数
extern LVar* locals;
// ローカル変数のカウンタ
extern int local_count;

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char* fmt, ...);
// エラー箇所を報告する
void error_at(char* loc, char* fmt, ...);
// 次のトークンが期待している記号の時には、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume(char* op);
// 次のトークンが識別子の時にはトークンを1つ読み進めて識別子のトークンを返す。
// それ以外の場合にはNULLを返す。
Token* consume_ident();
// 次のトークンが期待している記号の時には、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する
void expect(char* op);
// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number();
bool at_eof();

// 新しいトークンを作成してcurに繋げる
Token* new_token(TokenKind kind, Token* cur, char* str);
// 入力文字列pをトークナイズしてそれを返す
Token* tokenize(char* p);
Node* new_node(NodeKind kind, Node* lhs, Node* rhs);
Node* new_node_num(int val);
// 変数を検索する。見つからなかった場合はNULLを返す。
LVar* find_lvar(Token* token);

Node* program();
Node* stmt();
Node* expr();
Node* assign();
Node* equality();
Node* relational();
Node* add();
Node* mul();
Node* unary();
Node* primary();

void gen_lval(Node* node);
void gen(Node* node);