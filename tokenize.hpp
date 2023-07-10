#ifndef PCC_TOKENIZE_H
#define PCC_TOKENIZE_H

#include <stdint.h>


enum TokenKind
{
    TK_PUNCT,   // Punctuators
    TK_IDENT,   // Identifiers
    TK_KEYWORD, // Keywords
    TK_STR,     // String literals
    TK_NUM,
    TK_EOF
};


struct Type;

struct Token
{
    TokenKind kind;
    Token *next;
    int64_t val; // used if kind is TK_NUM
    char *loc;
    int len;
    Type *ty;    // used if TK_STR
    char *str;   // string literal contents
    int line_no; // line number
};


void error_at(char *loc, char *fmt, ...);
void error_tok(Token *tok, char *fmt, ...);
bool equal(Token *tok, char *op);
Token *skip(Token *tok, char *op);
bool consume(Token **rest, Token *tok, char *str);
Token *tokenize_file(char *filename);




#endif /* PCC_TOKENIZE_H */
