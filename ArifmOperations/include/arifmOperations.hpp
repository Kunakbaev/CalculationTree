#ifndef ARIFM_OPERATIONS_INCLUDE_ARIFM_OPERATIONS_HPP
#define ARIFM_OPERATIONS_INCLUDE_ARIFM_OPERATIONS_HPP

#include "arifmOperationsErrors.hpp"
#include "../../Dumper/include/dumperStruct.hpp"
#include "../../ArifmeticTree/include/arifmTreeNodeStruct.hpp"
#include "functionsRealizations.hpp"

struct Node2stringSettings {
    bool isNodeTypeNeeded;
    bool isFuncTypeNeeded;
    bool isBracketsNeeded;
};

ArifmOperationsErrors validateArifmOperationsArrays();
ArifmOperationsErrors initArifmTreeNodeWithString(Node* node, const char* line);
ArifmOperationsErrors getFunctionByName(const char* name, Function* func);
ArifmOperationsErrors getFuncByIndex(size_t funcIndex, Function* func);
size_t getVariableIndex(char ch);
ArifmOperationsErrors arifmTreeNodeDataToString(const Node* node, char** result,
                                                bool isFuncTypeNeeded);
ArifmOperationsErrors arifmTreeNodeToString(const Node* node, char** result,
                                            const Node2stringSettings* settings);
ArifmOperationsErrors getNodeLatexString(const Node* node, char* leftString, char* rightString, char** result);
const char* getArifmTreeNodeType(const Node* node);
bool isNeedForBrackets(const Node* parent, const Node* cur, bool isLeftSon);

#endif
