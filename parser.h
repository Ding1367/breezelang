//
// Created by Alife Zithu on 4/21/25.
//

#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

enum node_type_t {
    NODE_ROOT,

    NODE_IDENTIFIER, NODE_QUALIFIED_NAME,
    NODE_USING_STMT
};

typedef struct node_t node_t;
typedef enum node_type_t node_type_t;

typedef struct node_t {
    node_t *parent;
    node_type_t type;
} node_t;

typedef struct binary_node_t {
    node_t base;
    node_t *left, right;
} binary_node_t;

typedef struct identifier_node_t {
    node_t base;
    token_t id;
} identifier_node_t;

typedef struct qualified_name_t {
    binary_node_t base;
} qualified_name_t;

typedef struct unary_stmt_t {
    node_t base;
    node_t *node;
} unary_stmt_t;

typedef struct using_stmt_t {
    unary_stmt_t base;
} using_stmt_t;

typedef struct root_node_t root_node_t;

typedef struct parser_t {
    lexer_t *l;
    struct root_node_t {
        node_t base;
        size_t size, cap;
        node_t **children;
    } tree;
} parser_t;

node_t *parser_newnode_(size_t sz, node_t *parent, node_type_t tag);
#define parser_newnode(parent, type, tag) (type*)parser_newnode_(sizeof(type), parent, tag)

void parser_init(parser_t *ps, lexer_t *l);

#endif //PARSER_H
