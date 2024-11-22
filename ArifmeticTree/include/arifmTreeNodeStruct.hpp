#ifndef ARIFMETIC_TREE_INCLUDE_ARIFM_TREE_NODE_STRUCT_HPP
#define ARIFMETIC_TREE_INCLUDE_ARIFM_TREE_NODE_STRUCT_HPP

#include <cstddef>

enum TreeNodeType {
    ARIFM_TREE_NUMBER_NODE = 0,
    ARIFM_TREE_VAR_NODE    = 1,
    ARIFM_TREE_FUNC_NODE   = 2,
}; // ASK: maybe move to arifm operations submodule?

struct Node {
    TreeNodeType nodeType;
    union {
        size_t     data;
        double     doubleData;
    };
    size_t       left;         // left son
    size_t       right;        // right son
    size_t       memBuffIndex; // ASK: is this field necessary
    size_t       parent;       // index in mem buff array of node's parent
};

#endif
