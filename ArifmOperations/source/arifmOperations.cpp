#include <ctype.h>
#include <errno.h>

#include "../include/arifmOperations.hpp"
#include "../include/funcsToLatexString.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, ARIFM_OPERATIONS_INVALID_ARGUMENT, getArifmOperationsErrorMessage)

#define IF_ERR_RETURN(error) \
    do {\
        COMMON_IF_ERR_RETURN(error, getArifmOperationsErrorMessage, ARIFM_OPERATIONS_STATUS_OK);\
        /*DEBUG_VARS_TO_DUMPER_ALL_LOGS_FILE((Dumper*)&list->dumper, #error);\*/\
    } while(0) \

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getArifmOperationsErrorMessage)\



#define ARIFM_OPP_INFIX_FUNC( name, command) {#name, BINARY_FUNC, command##2numsFunc, command##FuncToLatex},
#define ARIFM_OPP_UNARY_FUNC( name, command) {#name, UNARY_FUNC,  command##Func,      command##FuncToLatex},
#define ARIFM_OPP_BINARY_FUNC(name, command) {#name, BINARY_FUNC, command##Func,      command##FuncToLatex},

Function functions[] = {
    #include "../include/functionsCodeGen/infixFunctionsPlainText.in"
    #include "../include/functionsCodeGen/unaryFunctionsRealizations.in"
    #include "../include/functionsCodeGen/binaryFunctionsRealizations.in"
};

#undef ARIFM_OPP_INFIX_FUNC
#undef ARIFM_OPP_UNARY_FUNC
#undef ARIFM_OPP_BINARY_FUNC

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

static const char* getFuncType(const Function* function) {
    assert(function != NULL);

    switch (function->type) {
        case BINARY_FUNC:
            return "binary";
        case UNARY_FUNC:
            return "unary";
        default:
            return "unknown";
    }
}

static double getVarValue(size_t varIndex) {
    assert(varIndex < NUM_OF_VARS);

    LOG_ERROR("-----------");
    LOG_DEBUG_VARS(varIndex, varValuesArr[varIndex]);
    return varValuesArr[varIndex];
}

ArifmOperationsErrors arifmTreeNodeDataToString(const Node* node, char** result) {
    IF_ARG_NULL_RETURN(node);
    IF_ARG_NULL_RETURN(result);

    const size_t BUFF_SIZE = 1 << 8;
    *result = (char*)calloc(BUFF_SIZE, sizeof(char));
    IF_NOT_COND_RETURN(*result != NULL, ARIFM_OPERATIONS_MEMORY_ALLOCATION_ERROR);

    Function func = {};
    switch (node->nodeType) {
        case ARIFM_TREE_FUNC_NODE:
            assert(node->data < NUM_OF_FUNCS);
            func = functions[node->data];
            snprintf(*result, BUFF_SIZE, "%s, %s",
                     getFuncType(&func), func.name);
            return ARIFM_OPERATIONS_STATUS_OK;
        case ARIFM_TREE_NUMBER_NODE:
            snprintf(*result, BUFF_SIZE, "%g", node->doubleData);
            return ARIFM_OPERATIONS_STATUS_OK;
        case ARIFM_TREE_VAR_NODE:
            double varValue = getVarValue(node->data);
            snprintf(*result, BUFF_SIZE, "%c", 'a' + node->data);
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
    return ch - 'a';
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

ArifmOperationsErrors getFuncByIndex(size_t funcIndex, Function* func) {
    IF_ARG_NULL_RETURN(func);
    IF_NOT_COND_RETURN(funcIndex < NUM_OF_FUNCS,
                       ARIFM_OPERATIONS_INVALID_ARGUMENT);

    *func = functions[funcIndex];

    return ARIFM_OPERATIONS_STATUS_OK;
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

    // TODO: add appropriate error
    char* end_ptr = NULL;
    errno = 0;
    node->doubleData = strtod(line, &end_ptr); // TODO: get command or variable index
    LOG_DEBUG_VARS(line, end_ptr);
    IF_NOT_COND_RETURN(errno == 0 && strlen(end_ptr) == 0,
                       ARIFM_OPERATIONS_STR_TO_DOUBLE_ERROR); // ASK: how to fix this?
    node->nodeType = ARIFM_TREE_NUMBER_NODE;

    return ARIFM_OPERATIONS_STATUS_OK;
}

ArifmOperationsErrors getNodeLatexString(const Node* node, char* leftString, char* rightString,
                                              char** result) {
    IF_ARG_NULL_RETURN(node);
    IF_ARG_NULL_RETURN(result);

    Function func = {};
    switch (node->nodeType) {
        case ARIFM_TREE_NUMBER_NODE:
        case ARIFM_TREE_VAR_NODE:
            IF_ERR_RETURN(arifmTreeNodeDataToString(node, result));
            break;
        case ARIFM_TREE_FUNC_NODE:
            IF_ERR_RETURN(getFuncByIndex(node->data, &func));
            IF_ERR_RETURN((func.latexToStringFunc)(leftString, rightString, result));
            break;
        default:
            return ARIFM_OPERATIONS_INVALID_ARGUMENT; // TODO: add error
    }

    return ARIFM_OPERATIONS_STATUS_OK;
}

