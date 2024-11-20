#ifndef ARIFMETIC_TREE_INCLUDE_ARIFM_TREE_HPP
#define ARIFMETIC_TREE_INCLUDE_ARIFM_TREE_HPP

#include "arifmTreeErrors.hpp"
#include "../../Dumper/include/dumperStruct.hpp"
#include "../../ArifmOperations/include/arifmOperations.hpp"

enum TreeNodeType {
    ARIFM_TREE_NUMBER_NODE,
    ARIFM_TREE_VAR_NODE,
    ARIFM_TREE_FUNC_NODE,
};

struct Node {
    TreeNodeType nodeType;
    size_t       data;
    size_t       left;         // left son
    size_t       right;        // right son
    size_t       memBuffIndex; // ASK: is this field necessary
    size_t       parent;       // index in mem buff array of node's parent
};

struct ArifmTree {
    size_t                       root;
    Node*                        memBuff;
    size_t                       memBuffSize;
    size_t                       freeNodeIndex;
    Dumper                       dumper;
};

ArifmTreeErrors constructArifmTree(ArifmTree* tree, Dumper* dumper);

ArifmTreeErrors getNewNode(ArifmTree* tree, size_t* newNodeIndex);


//ArifmTreeErrors getPathToArifmTreeNode(const ArifmTree* tree, const Node* node, size_t** path);
ArifmTreeErrors readArifmTreeFromFile(ArifmTree* tree, const char* fileName);
ArifmTreeErrors saveArifmTreeToFile  (ArifmTree* tree, const char* fileName);

ArifmTreeErrors arifmTreeNodeToString(const Node* node, char** result);
ArifmTreeErrors dumpArifmTree(ArifmTree* tree);
ArifmTreeErrors openImageOfCurrentStateArifmTree(ArifmTree* tree);

ArifmTreeErrors isNodeAleftSonOfParent(const ArifmTree* tree, size_t parentInd, size_t vertInd, bool* is);
ArifmTreeErrors destructArifmTree(ArifmTree* tree);

#endif
