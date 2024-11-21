#ifndef ARIFM_OPERATIONS_INCLUDE_ARIFM_OPERATIONS_HPP
#define ARIFM_OPERATIONS_INCLUDE_ARIFM_OPERATIONS_HPP

#include "arifmOperationsErrors.hpp"
#include "../../Dumper/include/dumperStruct.hpp"
#include "../../ArifmeticTree/include/arifmTreeNodeStruct.hpp"
#include "functionsRealizations.hpp"

struct Function {
    const char*             name;
    functionsRealizationPtr func;
};

ArifmOperationsErrors initArifmTreeNodeWithString(Node* node, const char* line);
const char* getArifmTreeNodeType(const Node* node);
ArifmOperationsErrors arifmTreeNodeDataToString(const Node* node, char** result);
ArifmOperationsErrors arifmTreeNodeToString(const Node* node, char** result);

#endif
