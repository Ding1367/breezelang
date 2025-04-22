//
// Created by Alife Zithu on 4/21/25.
//

#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

enum token_type_t {
    TK_EOF,

    TK_NAME, TK_STRING, TK_NUMBER, TK_CHAR,

    // one character tokens start
    TK_COLON, TK_LEFT_DIAMOND, TK_RIGHT_DIAMOND, TK_LEFT_SQ, TK_RIGHT_SQ, TK_LEFT_CURLY,
    TK_RIGHT_CURLY, TK_LEFT_PAREN, TK_RIGHT_PAREN, TK_DOT, TK_EXCLAMATION, TK_COMMA,
    // one character tokens end
    TK_ELLIPSE, TK_STATIC_SEPARATOR /* :: */,

    TK_FUNC, TK_FOR, TK_USING
};

#define LEX_FIRST_RESERVED_TYPE (TK_FUNC)
#define NUM_RESERVED (TK_USING - LEX_FIRST_RESERVED_TYPE + 1)

extern const char *lex_reserved[NUM_RESERVED];

typedef struct lexer_t lexer_t;
typedef struct token_t token_t;
typedef struct loc_t loc_t;
typedef enum token_type_t token_type_t;

struct loc_t {
    int line, col, bufPos;
    const char *filename;
};

struct token_t {
    token_type_t type;
    size_t len;
    const char *id;
    loc_t where;
};

struct lexer_t {
    size_t len;
    const char *buf;
    loc_t where;
    token_t lookahead;
    loc_t lookahead_loc;
};

#ifndef __cplusplus
#define _Restrict restrict
#else
#define _Restrict
extern "C" {
#endif
void lex_init(lexer_t *l, const char *filename, const char *buf);
token_t lex_next(lexer_t *l);
token_t lex_peek(lexer_t *l, unsigned int n);
int lex_consume(lexer_t* l);
void lex_consumespaces(lexer_t *l);
int lex_peekc(const lexer_t* l, int n);
void lexU_error(const lexer_t *l, loc_t wherestr, const char *_Restrict header, const char *_Restrict footer);
#define lexU_errorhere(l, header, footer) lexU_error(l, l->where, header, footer)
#ifdef __cplusplus
}
#endif

#endif //LEXER_H
