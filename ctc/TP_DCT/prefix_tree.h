#ifndef PREFIX_TREE_H
#define PREFIX_TREE_H

#include <stdio.h>
#include "bit.h"

struct Node {
    int value; // Only useful in leaves, index of the prefix if a leaf, -1 otherwise
    struct Node* children[2];
};

struct Node* create_node();

Booleen is_leaf(struct Node* node);

void encode_prefix(struct Node* root, const char* prefix, int index);

struct Node* create_prefix_tree(char* prefixes[], unsigned int count);

void free_node(struct Node* node);

static struct Node* prefix_tree = NULL;

#endif
