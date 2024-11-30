#ifndef ARIFMETIC_TREE_INCLUDE_ARIFM_TREE_HPP
#define ARIFMETIC_TREE_INCLUDE_ARIFM_TREE_HPP

#include "arifmTreeErrors.hpp"
#include "../../Dumper/include/dumperStruct.hpp"
#include "../../ArifmOperations/include/arifmOperations.hpp"
#include "arifmTreeTreeStruct.hpp"

ArifmTreeErrors constructArifmTree(ArifmTree* tree, const Dumper* dumper);

ArifmTreeErrors getNewNode(ArifmTree* tree, size_t* newNodeIndex);


//ArifmTreeErrors getPathToArifmTreeNode(const ArifmTree* tree, const Node* node, size_t** path);
ArifmTreeErrors readArifmTreeFromFile(ArifmTree* tree, const char* fileName);
ArifmTreeErrors saveArifmTreeToFile  (const ArifmTree* tree, const char* fileName);

// static ArifmTreeErrors constructNodeWithKids(ArifmTree* tree, size_t* newNodeInd, TreeNodeType nodeType, const void* data,
//                                              size_t leftSon, size_t rightSon);
Node* getArifmTreeNodePtr(const ArifmTree* tree, size_t nodeInd);
size_t constructNodeWithKidsNoErrors(ArifmTree* tree, TreeNodeType nodeType, const NodeDataUnion data,
                                     size_t leftSon,  size_t rightSon);
ArifmTreeErrors validateArifmTree(const ArifmTree* tree);
ArifmTreeErrors dumpArifmTreeInConsole(const ArifmTree* tree, size_t nodeIndex,
                                       char** outputBuffer, const Node2stringSettings* settings);
ArifmTreeErrors dumpArifmTree(ArifmTree* tree);
ArifmTreeErrors openImageOfCurrentStateArifmTree(ArifmTree* tree);

ArifmTreeErrors simplifyTree(ArifmTree* tree);
ArifmTreeErrors getDerivativeOfTree(const ArifmTree* source, ArifmTree* dest);
ArifmTreeErrors destructArifmTree(ArifmTree* tree);

#endif
