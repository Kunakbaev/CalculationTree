#include <ctype.h>

#include "../include/arifmOperations.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, ARIFM_OPERATIONS_INVALID_ARGUMENT, getArifmOperationsErrorMessage)

#define IF_ERR_RETURN(error) \
    do {\
        COMMON_IF_ERR_RETURN(error, getArifmOperationsErrorMessage, ARIFM_OPERATIONS_STATUS_OK);\
        /*DEBUG_VARS_TO_DUMPER_ALL_LOGS_FILE((Dumper*)&list->dumper, #error);\*/\
    } while(0) \

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getArifmOperationsErrorMessage)\


#define FUNCTION(name, funcPtr) \
    {#name, funcPtr},

Function functions[] = {
    #include "../include/functionsPlainText.in"
};

#undef FUNCTION

const size_t NUM_OF_FUNCS = sizeof(functions) / sizeof(*functions);
const size_t NUM_OF_VARS  = 26;

double varValuesArr[NUM_OF_VARS] = {};

const char* getArifmTreeNodeType(const Node* node) {
    assert(node != NULL);

    switch (node->nodeType) {
        case ARIFM_TREE_VAR_NODE:
            return "variable";
        case ARIFM_TREE_NUMBER_NODE:
            return "number";
        case ARIFM_TREE_FUNC_NODE:
            return "function";
        default:
            return "unknown";
    }
}

static double getVarValue(size_t varIndex) {
    assert(varIndex < NUM_OF_VARS);

    return varValuesArr[varIndex];
}

ArifmOperationsErrors arifmTreeNodeDataToString(const Node* node, char** result) {
    IF_ARG_NULL_RETURN(node);
    IF_ARG_NULL_RETURN(result);

    const size_t BUFF_SIZE = 1 << 8;
    *result = (char*)calloc(BUFF_SIZE, sizeof(char));
    IF_NOT_COND_RETURN(*result != NULL, ARIFM_OPERATIONS_MEMORY_ALLOCATION_ERROR);

    switch (node->nodeType) {
        case ARIFM_TREE_FUNC_NODE:
            assert(node->data < NUM_OF_FUNCS);
            strcpy(*result, functions[node->data].name);
            return ARIFM_OPERATIONS_STATUS_OK;
        case ARIFM_TREE_NUMBER_NODE:
            snprintf(*result, BUFF_SIZE, "%g", node->doubleData);
            return ARIFM_OPERATIONS_STATUS_OK;
        case ARIFM_TREE_VAR_NODE:
            double varValue = getVarValue(node->data);
            snprintf(*result, BUFF_SIZE, "%g", node->data);
            return ARIFM_OPERATIONS_STATUS_OK;
    }

    return ARIFM_OPERATIONS_STATUS_OK;
}

ArifmOperationsErrors arifmTreeNodeToString(const Node* node, char** result) {
    IF_ARG_NULL_RETURN(node);

    const size_t BUFF_SIZE = 1 << 9;
    *result = (char*)calloc(BUFF_SIZE, sizeof(char));
    IF_NOT_COND_RETURN(*result != NULL, ARIFM_OPERATIONS_MEMORY_ALLOCATION_ERROR);
    char* buffPtr = *result;

    const char* nodeType = getArifmTreeNodeType(node);
    char*       nodeData = NULL;
    IF_ERR_RETURN(arifmTreeNodeDataToString(node, &nodeData));
    LOG_DEBUG_VARS(nodeData, nodeType);
    buffPtr += snprintf(buffPtr, BUFF_SIZE - (buffPtr - *result),
                        "(%s, %s)", nodeType, nodeData);
    FREE(nodeData);

    return ARIFM_OPERATIONS_STATUS_OK;
}

static size_t getVariableIndex(char ch) {
    return ch - 'a' + 1; // is +1 necessary?
}

// superSlow
static ArifmOperationsErrors getFunctionIndex(const char* funcName, size_t* result) {
    IF_ARG_NULL_RETURN(funcName);
    IF_ARG_NULL_RETURN(result);

    for (size_t funcInd = 0; funcInd < NUM_OF_FUNCS; ++funcInd) {
        if (strcmp(functions[funcInd].name, funcName) == 0) {
            *result = funcInd;
            return ARIFM_OPERATIONS_STATUS_OK;
        }
    }

    return ARIFM_OPERATIONS_FUNC_NOT_FOUND;
}

static bool isStringVar(const char* line) {
    return strlen(line) == 1 &&
              'a' <= line[0] && line[0] <= 'z';
}

ArifmOperationsErrors initArifmTreeNodeWithString(Node* node, const char* line) {
    assert(node != NULL);
    assert(line != NULL);

    if (isStringVar(line)) {
        node->nodeType = ARIFM_TREE_VAR_NODE;
        node->data     = getVariableIndex(line[0]);
        return ARIFM_OPERATIONS_STATUS_OK;
    }

    ArifmOperationsErrors err = getFunctionIndex(line, &node->data);
    if (err == ARIFM_OPERATIONS_STATUS_OK) {
        node->nodeType = ARIFM_TREE_FUNC_NODE;
        return ARIFM_OPERATIONS_STATUS_OK;
    }

    // TODO: add error
    char* end_ptr = NULL;
    node->doubleData = strtod(line, &end_ptr); // TODO: get command or variable index
    node->nodeType = ARIFM_TREE_NUMBER_NODE;

    return ARIFM_OPERATIONS_STATUS_OK;
}


