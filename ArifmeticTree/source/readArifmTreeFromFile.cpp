#include <ctype.h>

#include "../include/arifmTree.hpp"
#include "../../ArifmOperations/include/arifmOperations.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, ARIFM_TREE_INVALID_ARGUMENT, getArifmTreeErrorMessage)

#define IF_ERR_RETURN(error) \
    do {\
        COMMON_IF_ERR_RETURN(error, getArifmTreeErrorMessage, ARIFM_TREE_STATUS_OK);\
        /*DEBUG_VARS_TO_DUMPER_ALL_LOGS_FILE((Dumper*)&list->dumper, #error);\*/\
    } while(0) \

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getArifmTreeErrorMessage)\

#define DUMPER_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getDumperErrorMessage, DUMPER_STATUS_OK, ARIFM_TREE_DUMPER_ERROR);

#define ARIFM_OPS_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getArifmOperationsErrorMessage, ARIFM_OPERATIONS_STATUS_OK, ARIFM_TREE_ARIFM_OPS_ERROR);

static const size_t BUFF_SIZE = 1 << 10;
static const char*  BAD_SYMBOLS = " \t\n";

static bool isBadSymbol(char ch) {
    return strchr(BAD_SYMBOLS, ch) != NULL;
}

static ArifmTreeErrors removeGarbageFromInputString(char* buffer, char** result) {
    IF_ARG_NULL_RETURN(buffer);
    IF_ARG_NULL_RETURN(result);

    size_t buffLen = strlen(buffer);
    *result = (char*)calloc(buffLen + 1, sizeof(buffer));
    IF_NOT_COND_RETURN(*result != NULL,
                        ARIFM_TREE_MEMORY_ALLOCATION_ERROR);
    for (size_t buffInd = 0, resInd = 0; buffInd < buffLen; ++buffInd) {
        char ch = buffer[buffInd];
        if (isBadSymbol(ch))
            continue;

        (*result)[resInd++] = ch;
    }

    return ARIFM_TREE_STATUS_OK;
}

static size_t getCharBalanceDx(char ch) {
    if (ch == '(') return 1;
    if (ch == ')') return -1;
    return 0;
}

struct Pair {
    size_t first;
    size_t second;
};

static ArifmTreeErrors findCommandSubstrSegm(const char* line, size_t len,
                                             Pair* commandSegm, Pair* one, Pair* two) {
    IF_ARG_NULL_RETURN(line);
    IF_ARG_NULL_RETURN(commandSegm);
    IF_ARG_NULL_RETURN(one);
    IF_ARG_NULL_RETURN(two);

    // TODO: add check for correct input string
    size_t commaInd = -1;
    size_t balance  = 0; // file_to_tree
    //LOG_DEBUG_VARS(line, len);
    *commandSegm = *one = *two = {1, 0};
    for (size_t i = 0; i < len; ++i) {
        char ch = line[i];
        size_t dx = getCharBalanceDx(ch);
        balance += dx;

        //LOG_DEBUG_VARS(*left, *right, balance, dx, ch);
        if (ch == ',' && balance == 1)
            commaInd = i;
        if (balance == 0 && dx == 0) {
            if (commandSegm->first == 1 && commandSegm->second == 0)
                commandSegm->first = i;
            commandSegm->second = i;
        }
    }

    if (commaInd != -1) {
        *one = {commandSegm->second + 2, commaInd - 1};
        *two = {commaInd + 1           , len - 2};
    } else {
        *one = {1                      , commandSegm->first - 2};
        *two = {commandSegm->second + 2, len - 2};
    }

    // TODO: add check that there is exactly one substring with zero brackets balance
    return ARIFM_TREE_STATUS_OK;
}

ArifmTreeErrors linkNewNodeToParent(ArifmTree* tree, size_t parentInd, bool isLeftSon,
                                    size_t* newNodeInd, const char* substr) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(newNodeInd);
    IF_ARG_NULL_RETURN(substr);

    *newNodeInd = 0;
    IF_ERR_RETURN(getNewNode(tree, newNodeInd));
    if (tree->root == 0) // tree is empty
        tree->root = *newNodeInd;
    Node* node = &tree->memBuff[*newNodeInd];

    if (parentInd != 0) {
        node->parent = parentInd;
        Node* parent = &tree->memBuff[parentInd];
        if (isLeftSon)
            parent->left  = *newNodeInd;
        else
            parent->right = *newNodeInd;
    }

    ARIFM_OPS_ERR_CHECK(initArifmTreeNodeWithString(node, substr));
    LOG_DEBUG_VARS((node)->data, (node)->nodeType);

    return ARIFM_TREE_STATUS_OK;
}

static ArifmTreeErrors recursiveStringParseToArifmTree(ArifmTree* tree, size_t parentInd, bool isLeftSon,
                                                const char* line, size_t lineLen) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(line);
    if (lineLen == 0)
        return ARIFM_TREE_STATUS_OK;
    // IF_NOT_COND_RETURN(lineLen != 0, ARIFM_TREE_INVALID_ARGUMENT); // maybe not error

    Pair commandSegm  = {};
    Pair leftSonSegm  = {};
    Pair rightSonSegm = {};
    IF_ERR_RETURN(findCommandSubstrSegm(line, lineLen, &commandSegm,
                                        &leftSonSegm, &rightSonSegm));
    // TODO: add assert
    // assert(left < lineLen);
    // assert(right < lineLen);

    // TODO: redo this
    size_t substrLen = commandSegm.second - commandSegm.first + 1;
    char* substr = (char*)calloc(substrLen + 1, sizeof(char));
    IF_NOT_COND_RETURN(substr != NULL, ARIFM_TREE_MEMORY_ALLOCATION_ERROR);
    strncpy(substr, line + commandSegm.first, substrLen);
    LOG_DEBUG_VARS(substr);

    size_t newNodeInd = 0;
    IF_ERR_RETURN(linkNewNodeToParent(tree, parentInd, isLeftSon, &newNodeInd, substr));
    LOG_DEBUG_VARS(commandSegm.first, commandSegm.second);
    LOG_DEBUG_VARS(leftSonSegm.first, leftSonSegm.second);
    LOG_DEBUG_VARS(rightSonSegm.first, rightSonSegm.second);
    LOG_DEBUG_VARS(line, lineLen);

    if (leftSonSegm.first <= leftSonSegm.second && leftSonSegm.second < lineLen) {
        IF_ERR_RETURN(recursiveStringParseToArifmTree(tree, newNodeInd, true,
                                                      line + leftSonSegm.first,
                                                      leftSonSegm.second - leftSonSegm.first + 1));
    }
    if (rightSonSegm.first <= rightSonSegm.second && rightSonSegm.second < lineLen) {
        IF_ERR_RETURN(recursiveStringParseToArifmTree(tree, newNodeInd, false,
                                                      line + rightSonSegm.first,
                                                      rightSonSegm.second - rightSonSegm.first + 1));
    }

    FREE(substr);
    return ARIFM_TREE_STATUS_OK;
}

ArifmTreeErrors readArifmTreeFromFile(ArifmTree* tree, const char* fileName) {
    IF_ARG_NULL_RETURN(tree);

    FILE* file = fopen(fileName, "r");
    IF_NOT_COND_RETURN(file != NULL, ARIFM_TREE_FILE_OPENING_ERROR);

    char inputBuffer[BUFF_SIZE];
    fgets(inputBuffer, BUFF_SIZE, file);
    LOG_DEBUG_VARS(inputBuffer);

    char* goodString = NULL;
    IF_ERR_RETURN(removeGarbageFromInputString(inputBuffer, &goodString));

    size_t lineLen = strlen(goodString); // TODO: it is already has been counted in previous function
    IF_ERR_RETURN(recursiveStringParseToArifmTree(tree, 0, false, goodString, lineLen));

    fclose(file);
    FREE(goodString);

    return ARIFM_TREE_STATUS_OK;
}
