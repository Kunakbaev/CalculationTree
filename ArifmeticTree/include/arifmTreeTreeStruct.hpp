#ifndef ARIFMETIC_TREE_INCLUDE_ARIFM_TREE_TREE_STRUCT_HPP
#define ARIFMETIC_TREE_INCLUDE_ARIFM_TREE_TREE_STRUCT_HPP

#include "arifmTreeNodeStruct.hpp"
#include "../../Dumper/include/dumperStruct.hpp"

struct ArifmTree {
    size_t                       root;
    Node*                        memBuff;
    size_t                       memBuffSize;
    size_t                       freeNodeIndex;
    Dumper*                      dumper;
};

#endif
