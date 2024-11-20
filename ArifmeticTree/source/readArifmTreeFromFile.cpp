#include <ctype.h>

#include "../include/arifmTree.hpp"

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

const size_t BUFF_SIZE = 1 << 10;
const char*  BAD_SYMBOLS = " \t\n";

bool isBadSymbol(char ch) {
    return strchr(BAD_SYMBOLS, ch) != NULL;
}

ArifmTreeErrors removeGarbageFromInputString(char* buffer, char** result) {
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

size_t getCharbalanceDx(char ch) {
    if (ch == '(') return 1;
    if (ch == ')') return -1;
    return 0;
}

ArifmTreeErrors findCommandSubstrSegm(const char* line, size_t len, size_t* left, size_t* right) {
    IF_ARG_NULL_RETURN(line);
    IF_ARG_NULL_RETURN(left);
    IF_ARG_NULL_RETURN(right);

    // TODO: add check for correct input string
    *left = *right = -1;
    size_t balance = 0;
    //LOG_DEBUG_VARS(line, len);
    for (size_t i = 0; i < len; ++i) {
        char ch = line[i];
        size_t dx = getCharbalanceDx(ch);
        balance += dx;

        //LOG_DEBUG_VARS(*left, *right, balance, dx, ch);
        if (balance == 0 && dx == 0) {
            if (*left == -1)
                *left = i;
            *right = i;
        }
    }

    // TODO: add check that there is exactly one substring with zero brackets balance

    return ARIFM_TREE_STATUS_OK;
}

static void initArifmTreeNodeWithString(Node* node, const char* line) {
    assert(node != NULL);
    assert(line != NULL);

    size_t len = strlen(line);
    bool allDigits = true; // TODO: what to do with double??
    for (size_t i = 0; i < len; ++i)
        allDigits &= isdigit(line[i]);

    LOG_DEBUG_VARS(allDigits, len);
    if (!allDigits) {
        node->data = (size_t)line; // TODO: get command or variable index
        node->nodeType = len == 1 && isalpha(line[0]) ? ARIFM_TREE_VAR_NODE : ARIFM_TREE_FUNC_NODE;
    } else {
        node->data = atoi(line); // TODO: get command or variable index
        node->nodeType = ARIFM_TREE_NUMBER_NODE;
    }
}

ArifmTreeErrors recursiveStringParseToArifmTree(ArifmTree* tree, size_t parentInd, bool isLeftSon,
                                                const char* line, size_t lineLen) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(line);
    if (lineLen == 0)
        return ARIFM_TREE_STATUS_OK;
    // IF_NOT_COND_RETURN(lineLen != 0, ARIFM_TREE_INVALID_ARGUMENT); // maybe not error

    size_t left  = 0;
    size_t right = 0;
    IF_ERR_RETURN(findCommandSubstrSegm(line, lineLen, &left, &right));
    assert(left < lineLen);
    assert(right < lineLen);

    size_t newNodeInd = 0;
    IF_ERR_RETURN(getNewNode(tree, &newNodeInd));
    if (tree->root == 0) // tree is empty
        tree->root = newNodeInd;
    Node* node = &tree->memBuff[newNodeInd];
    LOG_DEBUG_VARS(left, right, line, lineLen);

    // TODO: redo this
    char substr[100] = {};
    strcpy(substr, line + left);
    substr[right - left + 1] = '\0';
    LOG_DEBUG_VARS(substr);

    LOG_DEBUG_VARS(left, right, substr);
    initArifmTreeNodeWithString(node, substr);
    LOG_DEBUG_VARS(node->data, node->nodeType);

    if (parentInd != 0) {
        node->parent = parentInd;
        Node* parent = &tree->memBuff[parentInd];
        if (isLeftSon)
            parent->left  = newNodeInd;
        else
            parent->right = newNodeInd;
    }

    if (left != 0) {
        IF_ERR_RETURN(recursiveStringParseToArifmTree(tree, newNodeInd, true, line + 1, left));
    }
    if (right != lineLen - 1) {
        IF_ERR_RETURN(recursiveStringParseToArifmTree(tree, newNodeInd, false, line + right + 2, lineLen - right - 3));
    }

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
    IF_ERR_RETURN(recursiveStringParseToArifmTree(tree, NULL, false, goodString, lineLen));

    fclose(file);
    FREE(goodString);

    return ARIFM_TREE_STATUS_OK;
}
