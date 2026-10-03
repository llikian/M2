#include "prefix_tree.h"

#include "bases.h"

struct Node* create_node() {
    struct Node* node;

    ALLOUER(node, 1);

    node->value = -1;
    node->children[0] = NULL;
    node->children[1] = NULL;

    return node;
}

Booleen is_leaf(struct Node* node) {
    return node->children[0] == NULL && node->children[1] == NULL;
}

void encode_prefix(struct Node* root, const char* prefix, int index) {
    struct Node* node = root;

    for(int i = 0; prefix[i] != '\0'; ++i) {
        int child_index = prefix[i] - '0';
        if(node->children[child_index] == NULL) {
            node->children[child_index] = create_node();
        }
        node = node->children[child_index];
    }

    node->value = index;
}

struct Node* create_prefix_tree(char* prefixes[], unsigned int count) {
    struct Node* root = create_node();

    for(int i = 0; i < count; ++i) {
        encode_prefix(root, prefixes[i], i);
    }

    return root;
}

void free_node(struct Node* node) {
    if(node != NULL) {
        if(node->children[0] != NULL) {
            free_node(node->children[0]);
        }
        if(node->children[1] != NULL) {
            free_node(node->children[1]);
        }
        free(node);
    }
}
