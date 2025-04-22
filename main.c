#include <stdio.h>

#include "lexer.h"

int main(int argc, char **argv) {
    FILE *f = fopen("main.bl", "r");
    char buf[4096];
    fread(buf, 1, 4096, f);

    lexer_t lex;
    lex_init(&lex, "main.bl", buf);

    token_t t;
    while (t = lex_next(&lex), t.type != TK_EOF);
    puts("");

    fclose(f);
    return 0;
}
