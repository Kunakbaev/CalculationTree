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



#define ARIFM_OPP_INFIX_FUNC( name, command, enumName, _) {BINARY_FUNC, ELEM_FUNC_##enumName, command##Func, command##FuncToLatex},
#define ARIFM_OPP_UNARY_FUNC( name, command, enumName, _) {UNARY_FUNC,  ELEM_FUNC_##enumName, command##Func, command##FuncToLatex},
#define ARIFM_OPP_BINARY_FUNC ARIFM_OPP_INFIX_FUNC

Function functions[] = {
    #include "../include/functionsCodeGen/binaryFunctionsRealizations.in"
    #include "../include/functionsCodeGen/infixFunctionsPlainText.in"
    #include "../include/functionsCodeGen/unaryFunctionsRealizations.in"
};

#undef ARIFM_OPP_INFIX_FUNC
#undef ARIFM_OPP_UNARY_FUNC
#undef ARIFM_OPP_BINARY_FUNC

#define ARIFM_OPP_GENERAL_FUNC( name, _, __, ___) #name,

const char* const functionNames[] = {
    #include "../include/functionsCodeGen/allFuncs.hpp"
};

#undef ARIFM_OPP_GENERAL_FUNC

const size_t NUM_OF_FUNCS = sizeof(functions) / sizeof(*functions);
static_assert((sizeof(functionNames) / sizeof(*functionNames)) == NUM_OF_FUNCS);
const size_t NUM_OF_VARS  = 26;

double varValuesArr[NUM_OF_VARS] = {};

ArifmOperationsErrors validateArifmOperationsArrays() {
    size_t funcNamesArrSize = sizeof(functionNames) / sizeof(*functionNames);
    IF_NOT_COND_RETURN(funcNamesArrSize == NUM_OF_FUNCS,
                        ARIFM_OPERATIONS_INVALID_FUNC_ARRAYS);
    for (size_t funcInd = 0; funcInd < NUM_OF_FUNCS; ++funcInd) {
        Function    func     = functions[funcInd];
        //const char* funcName = functionNames[funcInd];
        //LOG_DEBUG_VARS(func.name, funcInd);
        IF_NOT_COND_RETURN(func.name == funcInd          , ARIFM_OPERATIONS_INVALID_FUNC_ARRAYS);
        IF_NOT_COND_RETURN(func.calculationFunc   != NULL, ARIFM_OPERATIONS_INVALID_FUNC_ARRAYS);
        IF_NOT_COND_RETURN(func.latexToStringFunc != NULL, ARIFM_OPERATIONS_INVALID_FUNC_ARRAYS);
        IF_NOT_COND_RETURN(func.type == UNARY_FUNC || func.type == BINARY_FUNC,
                           ARIFM_OPERATIONS_INVALID_FUNC_ARRAYS);
    }

    return ARIFM_OPERATIONS_STATUS_OK;
}

static const char* getFuncName(FunctionsNames funcName) {
    assert(funcName < NUM_OF_FUNCS);
    return functionNames[funcName];
}

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

ArifmOperationsErrors getFunctionByName(const char* name, Function* func) {
    IF_ARG_NULL_RETURN(name);
    IF_ARG_NULL_RETURN(func);

    for (size_t funcInd = 0; funcInd < NUM_OF_FUNCS; ++funcInd) {
        //LOG_DEBUG_VARS(name, functionNames[funcInd]);
        if (strcmp(name, functionNames[funcInd]) == 0) {
            *func = functions[funcInd];
            return ARIFM_OPERATIONS_STATUS_OK;
        }
    }

    return ARIFM_OPERATIONS_FUNC_NOT_FOUND;
}
//
// static double getVarValue(size_t varIndex) {
//     assert(varIndex < NUM_OF_VARS);
//
//     LOG_DEBUG_VARS(varIndex, varValuesArr[varIndex]);
//     return varValuesArr[varIndex];
// }

ArifmOperationsErrors arifmTreeNodeDataToString(const Node* node, char** result,
                                                bool isFuncTypeNeeded) {
    IF_ARG_NULL_RETURN(node);
    IF_ARG_NULL_RETURN(result);

    const size_t BUFF_SIZE = 1 << 8;
    *result = (char*)calloc(BUFF_SIZE, sizeof(char));
    IF_NOT_COND_RETURN(*result != NULL, ARIFM_OPERATIONS_MEMORY_ALLOCATION_ERROR);
    char* resultPtr = *result;

    #define ADD2BUFF(format, ...)                                                 \
        do {                                                                      \
            resultPtr += snprintf(resultPtr, BUFF_SIZE - (resultPtr - *result),   \
                                  format, __VA_ARGS__);                           \
        } while (0)

    Function func        = {};
    const char* funcName = NULL;
    switch (node->nodeType) {
        case ARIFM_TREE_FUNC_NODE:
            assert(node->data < NUM_OF_FUNCS);
            func     = functions[node->data];
            funcName = getFuncName(func.name);

            if (isFuncTypeNeeded) ADD2BUFF("%s, %s", getFuncType(&func), funcName);
            else                  ADD2BUFF("%s",     funcName);

            return ARIFM_OPERATIONS_STATUS_OK;
        case ARIFM_TREE_NUMBER_NODE:
            ADD2BUFF("%g", node->doubleData);
            return ARIFM_OPERATIONS_STATUS_OK;
        case ARIFM_TREE_VAR_NODE:
            ADD2BUFF("%c", 'a' + node->data);
            return ARIFM_OPERATIONS_STATUS_OK;
    }

    return ARIFM_OPERATIONS_STATUS_OK;
}

ArifmOperationsErrors arifmTreeNodeToString(const Node* node, char** result,
                                            const Node2stringSettings* settings) {
    IF_ARG_NULL_RETURN(node);
    IF_ARG_NULL_RETURN(settings);

    const size_t BUFF_SIZE = 1 << 9;
    *result = (char*)calloc(BUFF_SIZE, sizeof(char));
    IF_NOT_COND_RETURN(*result != NULL, ARIFM_OPERATIONS_MEMORY_ALLOCATION_ERROR);
    char* buffPtr = *result;

    const char* nodeType = getArifmTreeNodeType(node);
    char*       nodeData = NULL;

    #define ADD2BUFF(format, ...)                                           \
        do {                                                                \
            buffPtr += snprintf(buffPtr, BUFF_SIZE - (buffPtr - *result),   \
                                format, __VA_ARGS__);                       \
        } while (0)

    IF_ERR_RETURN(arifmTreeNodeDataToString(node, &nodeData, settings->isFuncTypeNeeded));
    if (settings->isBracketsNeeded) ADD2BUFF("%s", "(");
    if (settings->isNodeTypeNeeded) ADD2BUFF("%s, %s", nodeType, nodeData);
    else                            ADD2BUFF("%s", nodeData);
    if (settings->isBracketsNeeded) ADD2BUFF("%s", ")");
    FREE(nodeData);

    return ARIFM_OPERATIONS_STATUS_OK;
}

size_t getVariableIndex(char ch) {
    return ch - 'a';
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

    Function func = {};
    ArifmOperationsErrors err = getFunctionByName(line, &func);
    if (err == ARIFM_OPERATIONS_STATUS_OK) {
        node->nodeType = ARIFM_TREE_FUNC_NODE;
        node->data     = func.name;
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

bool isNeedForBrackets(const Node* parent, const Node* cur, bool isLeftSon) {
    IF_ARG_NULL_RETURN(parent);
    IF_ARG_NULL_RETURN(cur);

    if (parent->nodeType != ARIFM_TREE_FUNC_NODE ||
        cur->nodeType    != ARIFM_TREE_FUNC_NODE)
            return false;

    Function parentFunc = {};
    Function curFunc    = {};
    IF_ERR_RETURN(getFuncByIndex(parent->data, &parentFunc));
    IF_ERR_RETURN(getFuncByIndex(cur->data,    &curFunc));

    FunctionsNames parName = parentFunc.name;
    FunctionsNames curName = curFunc.name;
    bool isHighPrior = parName == ELEM_FUNC_MUL || parName == ELEM_FUNC_DIV ||
                       parName == ELEM_FUNC_POW;
    bool isLowPrior  = curName == ELEM_FUNC_ADD || curName == ELEM_FUNC_SUB;
    return
        (isHighPrior && isLowPrior) ||
        (parName == ELEM_FUNC_POW && isLeftSon);
}

ArifmOperationsErrors getNodeLatexString(const Node* node, char* leftString, char* rightString,
                                              char** result) {
    IF_ARG_NULL_RETURN(node);
    IF_ARG_NULL_RETURN(result);

    Function func = {};
    switch (node->nodeType) {
        case ARIFM_TREE_NUMBER_NODE:
        case ARIFM_TREE_VAR_NODE:
            IF_ERR_RETURN(arifmTreeNodeDataToString(node, result, false));
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

