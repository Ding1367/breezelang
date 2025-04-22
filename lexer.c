//
// Created by Alife Zithu on 4/21/25.
//

#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define error(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__, NULL)

const char *lex_reserved[NUM_RESERVED] = {
    "func", "for", "using"
};

void lex_init(lexer_t* l, const char *filename, const char *buf) {
    l->lookahead.type = TK_EOF;
    l->where = (loc_t){1, 1, 0, filename};
    l->buf = buf;
    l->len = strlen(buf);
}

#define LEX_PAIR_CASE(tvar, l_const, lchar, rchar) case lchar: tvar.type = l_const; break; case rchar: tvar.type = l_const + 1; break;

#ifdef LEX_DEBUG
static const char *tokenTypeNames[] = {
    "EOF",
    "NAME", "STR", "NUM", "CHR",
    ":", "<", ">", "[", "]", "{", "}", "(", ")", ".", "!", ",",
    "...", "::", "func", "for", "using"
};
#endif

token_t lex_next(lexer_t* l) {
    token_t ahead = l->lookahead;
    if (ahead.type != TK_EOF) {
        l->where = l->lookahead_loc;
        l->lookahead.type = TK_EOF;
        return ahead;
    }
    int next = lex_peekc(l, 0);
    if (isspace(next)) {
        lex_consumespaces(l);
        return lex_next(l);
    }
    if (next == ';') {
        lex_consume(l);
        next = lex_peekc(l, 0);
    }
    if (next == '/' && lex_peekc(l, 1) == '/') {
        while (lex_consume(l) != '\n'){}
        next = lex_peekc(l, 0);
    }
    token_t t;
    t.type = TK_EOF;
    t.where = l->where;
    t.len = 0;
    t.id = NULL;
    if (isalpha(next)) {
        const char *start = &l->buf[l->where.bufPos];
        size_t len = 0;
        while (isalnum(lex_peekc(l, 0))) {
            len++;
            lex_consume(l);
        }
        t.type = TK_NAME;
        for (int i = 0; i < NUM_RESERVED; i++) {
            const char *rsvCst = lex_reserved[i];
            if (strlen(rsvCst) == len && strncmp(start, lex_reserved[i], len) == 0) {
                t.type = LEX_FIRST_RESERVED_TYPE + i;
            }
        }
        if (t.type == TK_NAME) {
            t.len = len;
            t.id = start;
        }
    } else if (next == '"') {
        lex_consume(l);
        // lex string until '"'
        const char *start = &l->buf[l->where.bufPos];
        size_t len = 0;
        int strc;
        while (strc = lex_peekc(l, 0), strc != '"') {
            if (strc == EOF) {
                lexU_errorhere(l, "unterminated string literal", NULL);
            }
            if (strc == '\n') {
                lexU_error(l, t.where, "string contains newline", NULL);
            }
            len++;
            lex_consume(l);
        }
        lex_consume(l);
        t.type = TK_STRING;
        t.len = len;
        t.id = start;
    } else if (next == '\'') {
        lex_consume(l);
        t.len = 1;
        t.id = &l->buf[l->where.bufPos];
        lex_consume(l);
        if (lex_consume(l) != '\'') {
            lexU_errorhere(l, "unterminated char literal", NULL);
        }
    } else if (isdigit(next)) {
        const char *start = &l->buf[l->where.bufPos];
        size_t len = 0;
        int f = 0;
        int strc;
        while ((strc = lex_peekc(l, 0))) {
            if (strc == '.') {
                if (f) {
                    lexU_errorhere(l, "multiple dots in numeric literal", NULL);
                }
                f = 1;
            }
            if (isalpha(strc)) {
                switch (strc) {
                case 'f': break;
                case 'u': break;
                case 'l':
                    if (lex_peekc(l, 1) == 'l') {
                        lex_consume(l);
                        len++;
                        if (lex_peekc(l, 2) == 'u') {
                            len++;
                        }
                    } else if (lex_peekc(l, 1) == 'u') {
                        lex_consume(l);
                        len++;
                    }
                    break;
                default:
                    lexU_errorhere(l, "unknown type hint", NULL);
                }
                len++;
                lex_consume(l);
                break;
            }
            if (!isdigit(strc) && strc != '.') {
                break;
            }
            len++;
            lex_consume(l);
        }
        t.type = TK_NUMBER;
        t.len = len;
        t.id = start;
    } else {
        // lex one char tokens
        lex_consume(l);
        switch (next) {
        LEX_PAIR_CASE(t, TK_LEFT_SQ, '[', ']')
        LEX_PAIR_CASE(t, TK_LEFT_DIAMOND, '<', '>')
        LEX_PAIR_CASE(t, TK_LEFT_PAREN, '(', ')')
        LEX_PAIR_CASE(t, TK_LEFT_CURLY, '{', '}')
        case ':': {
            if (lex_peekc(l, 0) == ':') {
                lex_consume(l);
                t.type = TK_STATIC_SEPARATOR;
                break;
            }
            t.type = TK_COLON; break;
        }
        case '.': {
            if (lex_peekc(l, 0) == '.') {
                lex_consume(l);
                lex_peekc(l, 1) == '.' ? (void)(t.type = TK_ELLIPSE) : lexU_errorhere(l, "unexpected extraneous period", NULL);
                lex_consume(l);
                break;
            }
            t.type = TK_DOT; break;
        }
        case '!': t.type = TK_EXCLAMATION; break;
        case ',': t.type = TK_COMMA; break;
        default: break;
        }
    }
#ifdef LEX_DEBUG
    if (t.id == NULL || t.len == 0) {
        printf("%s @ %s(%d:%d); ", tokenTypeNames[t.type], t.where.filename, t.where.line, t.where.col);
    } else {
        printf("%s{id=%zu(%*s)} @ %s(%d:%d); ", tokenTypeNames[t.type], t.len, (int)t.len, t.id, t.where.filename, t.where.line, t.where.col);
    }
#endif
    lex_consumespaces(l);
    return t;
}

token_t lex_peek(lexer_t* l, const unsigned int n) {
    if (n == 0 && l->lookahead.type != TK_EOF)
        return l->lookahead;
    lexer_t local = *l;
    token_t t = l->lookahead = lex_next(&local);
    l->lookahead_loc = local.where;
    for (int i = 0; i < n; i++)
        t = lex_next(&local);
    return t;
}

int lex_consume(lexer_t* l) {
    int p = l->where.bufPos++;
    int ch = p < l->len ? l->buf[p] : EOF;
    if (ch == '\n') {
        l->where.line++;
        l->where.col = 0;
    }
    l->where.col++;
    return ch;
}

void lex_consumespaces(lexer_t* l) {
    while (isspace(lex_peekc(l, 0))) lex_consume(l);
}

int lex_peekc(const lexer_t* l, int n) {
    size_t p = l->where.bufPos + n;
    if (p < l->len)
        return l->buf[p];
    return EOF;
}

void lexU_error(const lexer_t *l, const loc_t wherestr, const char* header, const char* footer) {
    int whereline = wherestr.line;
    int wherecol = wherestr.col;
    error("%s(%d:%d): %s\n", wherestr.filename, whereline, wherecol, header);

    const char *contents = l->buf;

    // this pointer should stay allocated for the length of the call
    const char *cline = contents;
    for (int i = 1; i < whereline; i++) {
        cline = strchr(cline, '\n');
        if (cline == NULL) {
            if (i == 1) {
                cline = contents;
                break;
            }
            fputs("Line doesn't exist???", stderr);
            abort();
        }
        cline++;
    }
    const char *end = strchr(cline, '\n');
    if (end == NULL) {
        // then this is the last line and it wasn't terminated with \n
        end = &cline[strlen(cline)];
    }
    const char *where = cline;
    size_t lineLength = 0;
    while (1) {
        if (*where == '\t') {
            fwrite("    ", 1, 4, stderr);
            lineLength += 4;
            where++;
        } else {
            const char *nextTab = strchr(where, '\t');
            if (nextTab == NULL) {
                fwrite(where, 1, end - where, stderr);
                lineLength += end - where;
                break;
            }
            const size_t distance = nextTab - where - 1;
            lineLength += distance;
            where = nextTab + 1;
        }
    }
    if (lineLength == 0) exit(1);
    fputs("\n", stderr);
    char *underline = malloc(lineLength + 1);
    if (underline) {
        memset(underline, '~', lineLength);
        underline[lineLength] = '\n';
        underline[wherecol - 1] = '^';
        fwrite(underline, 1, lineLength + 1, stderr);
        free(underline);
    } else {
        fprintf(stderr, "%*s\n", wherecol, "^");
    }
    if (footer != NULL) {
        fprintf(stderr, "\t%s\n", footer);
    }
    exit(1);
}
