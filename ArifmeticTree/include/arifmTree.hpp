#ifndef ARIFMETIC_TREE_INCLUDE_ARIFM_TREE_HPP
#define ARIFMETIC_TREE_INCLUDE_ARIFM_TREE_HPP

#include "arifmTreeErrors.hpp"
#include "../../Dumper/include/dumperStruct.hpp"
#include "../../ArifmOperations/include/arifmOperations.hpp"
#include "arifmTreeTreeStruct.hpp"

ArifmTreeErrors constructArifmTree(ArifmTree* tree, Dumper* dumper);

ArifmTreeErrors getNewNode(ArifmTree* tree, size_t* newNodeIndex);


//ArifmTreeErrors getPathToArifmTreeNode(const ArifmTree* tree, const Node* node, size_t** path);
ArifmTreeErrors readArifmTreeFromFile(ArifmTree* tree, const char* fileName);
ArifmTreeErrors saveArifmTreeToFile  (ArifmTree* tree, const char* fileName);

ArifmTreeErrors validateArifmTree(const ArifmTree* tree);
ArifmTreeErrors dumpArifmTree(ArifmTree* tree);
ArifmTreeErrors openImageOfCurrentStateArifmTree(ArifmTree* tree);

ArifmTreeErrors isNodeAleftSonOfParent(const ArifmTree* tree, size_t parentInd, size_t vertInd, bool* is);
ArifmTreeErrors destructArifmTree(ArifmTree* tree);

#endif
