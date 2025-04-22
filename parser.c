//
// Created by Alife Zithu on 4/21/25.
//

#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

node_t *parser_newnode_(size_t sz, node_t* parent, node_type_t tag) {
    node_t *node = malloc(sz);
    if (!node) {
        fprintf(stderr, "failed to allocate new node\n");
        abort();
    }
    node->parent = parent;
    node->type = tag;
    return node;
}

static void resize_tree(parser_t *ps) {
    root_node_t *tree = &ps->tree;
    tree->cap *= 1.75;
    tree->children = realloc(tree->children, tree->cap * sizeof(node_t *));
    if (!tree->children) {
        fprintf(stderr, "failed to resize tree\n");
        abort();
    }
}

void parser_init(parser_t* ps, lexer_t* l) {
    ps->tree = (root_node_t){NULL, NODE_ROOT, 5, 0, NULL};
    resize_tree(ps);
}

void parser_init(lexer_t* l) {
}
