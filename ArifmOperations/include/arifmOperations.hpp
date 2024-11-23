#ifndef ARIFM_OPERATIONS_INCLUDE_ARIFM_OPERATIONS_HPP
#define ARIFM_OPERATIONS_INCLUDE_ARIFM_OPERATIONS_HPP

#include "arifmOperationsErrors.hpp"
#include "../../Dumper/include/dumperStruct.hpp"
#include "../../ArifmeticTree/include/arifmTreeNodeStruct.hpp"
#include "functionsRealizations.hpp"

ArifmOperationsErrors initArifmTreeNodeWithString(Node* node, const char* line);
const char* getArifmTreeNodeType(const Node* node);
ArifmOperationsErrors getFuncByIndex(size_t funcIndex, Function* func);
ArifmOperationsErrors arifmTreeNodeDataToString(const Node* node, char** result);
ArifmOperationsErrors arifmTreeNodeToString(const Node* node, char** result);
ArifmOperationsErrors getNodeLatexString(const Node* node, char* leftString, char* rightString,
                                              char** result);

#endif
