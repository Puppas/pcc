#include "pcc.h"

static char *current_input;

void error(char *fmt, ...) 
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

static void verror_at(char *loc, char *fmt, va_list ap)
{
    int pos = loc - current_input;
    fprintf(stderr, "%s\n", current_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}


void error_at(char *loc, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    verror_at(loc, fmt, ap);
}


void error_tok(Token *tok, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verror_at(tok->loc, fmt, ap);
}


bool equal(Token *tok, char *op)
{
    return memcmp(tok->loc, op, tok->len) == 0 && op[tok->len] == '\0';
}


Token *skip(Token *tok, char *op)
{
    if(!equal(tok, op)){
        error_tok(tok, "expected '%s'", op);
    }
    return tok->next;
}


bool consume(Token **rest, Token *tok, char *str) {
    if (equal(tok, str)) {
        *rest = tok->next;
        return true;
    }
    *rest = tok;
    return false;
}


static Token *new_token(TokenKind kind, char *start, char *end)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->loc = start;
    tok->len = end - start;
    return tok;
}

static bool startswith(char *p, char *q)
{
    return strncmp(p, q, strlen(q)) == 0;
}

// returns true if c is valid as the first character of an identifier.
static bool is_ident1(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}


// returns true if c is valid as a non-first character of an identifier.
static bool is_ident2(char c) {
    return is_ident1(c) || ('0' <= c && c <= '9');
}


static int read_punct(char *p)
{
    if(startswith(p, "==") || startswith(p, "!=") ||
       startswith(p, "<=") || startswith(p, ">=")){
        return 2;
    }

    return ispunct(*p) ? 1 : 0;
}


static bool is_keyword(Token *tok) {
    static char *kw[] = {
        "return", "if", "else", "for", "while", "int",
        "sizeof", "char"};
    for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++)
        if (equal(tok, kw[i]))
            return true;

    return false;
}


static Token *read_string_literal(char *start) {
    char *p = start + 1;
    for(; *p != '"'; ++p) {
        if (*p == '\n' || *p == '\0')
            error_at(start, "unclosed string literal");
    }

    Token *tok = new_token(TK_STR, start, p + 1);
    tok->ty = array_of(ty_char, p - start);
    tok->str = strndup(start + 1, p - start - 1);
    return tok;
}



static void convert_keywords(Token *tok) {
    for (Token *t = tok; t->kind != TK_EOF; t = t->next)
        if (is_keyword(t))
            t->kind = TK_KEYWORD;
}


Token *tokenize(char *p)
{
    current_input = p;
    Token head = {};
    Token *cur = &head;

    while(*p)
    {
        if(isspace(*p)){
            ++p;
            continue;
        }        

        if(isdigit(*p)){
            cur = cur->next = new_token(TK_NUM, p, p);
            char *q = p;
            cur->val = strtoul(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        // string literal
        if(*p == '"') {
            cur = cur->next = read_string_literal(p);
            p += cur->len;
            continue;
        }

        // identifier or keyword
        if(is_ident1(*p)) {
            char *start = p;
            do {
                ++p;
            }while(is_ident2(*p));
            cur = cur->next = new_token(TK_IDENT, start, p);
            continue;
        }

        // punctuators
        int punct_len = read_punct(p);
        if(punct_len) {
            cur = cur->next = new_token(TK_PUNCT, p, p + punct_len);
            p += cur->len;
            continue;
        }

        error_at(p, "invalid token");
    }

    cur = cur->next = new_token(TK_EOF, p, p);
    convert_keywords(head.next);
    return head.next;
}