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

static ArifmTreeErrors recursiveTreeSaveToFile(ArifmTree* tree, size_t nodeInd, size_t parentInd, char** buffer) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(buffer);
    IF_NOT_COND_RETURN(nodeInd < tree->memBuffSize,
                       ARIFM_TREE_INVALID_ARGUMENT);

    Node node = tree->memBuff[nodeInd];

    // ASK: how to make this strings not NULL, but to still be able to free them
    char* leftSonString  = "";
    char* rightSonString = "";
    if (node.left != 0) {
        IF_ERR_RETURN(recursiveTreeSaveToFile(tree, node.left, nodeInd,  &leftSonString));
    }
    if (node.right != 0) {
        IF_ERR_RETURN(recursiveTreeSaveToFile(tree, node.right, nodeInd, &rightSonString));
    }

    ARIFM_OPS_ERR_CHECK(getNodeLatexString(&node, leftSonString, rightSonString, buffer));
    LOG_DEBUG_VARS(leftSonString);
    LOG_DEBUG_VARS(rightSonString);
    if (strlen(leftSonString) != 0)
        FREE(leftSonString);
    if (strlen(rightSonString) != 0)
        FREE(rightSonString);
    LOG_DEBUG_VARS(*buffer);

    if (parentInd != 0) {
        assert(parentInd < tree->memBuffSize);
        Node parent = tree->memBuff[parentInd];
        if (parent.nodeType == ARIFM_TREE_FUNC_NODE &&
            node.nodeType   == ARIFM_TREE_FUNC_NODE) {
            Function func = {};
            Function curFunc = {};
            ARIFM_OPS_ERR_CHECK(getFuncByIndex(parent.data, &func));
            ARIFM_OPS_ERR_CHECK(getFuncByIndex(node.data,   &curFunc));

            LOG_DEBUG_VARS(func.name, curFunc.name);
            // TODO:
            bool isHighPrior = strcmp(func.name, "*") == 0 || strcmp(func.name, "/") == 0 ||
                               strcmp(func.name, "^") == 0;
            bool isLowPrior  = strcmp(curFunc.name, "+") == 0 || strcmp(curFunc.name, "-") == 0;
            if (isHighPrior && isLowPrior) {
                char* tmp = (char*)calloc(strlen(*buffer) + 2 + 1, sizeof(char));
                IF_NOT_COND_RETURN(tmp != NULL, ARIFM_TREE_MEMORY_ALLOCATION_ERROR);

                sprintf(tmp, "(%s)", *buffer);
                FREE(*buffer);
                *buffer = tmp;
                LOG_DEBUG_VARS(*buffer);
            }
        }
    }

    return ARIFM_TREE_STATUS_OK;
}

static void saveTexBufferToFile(const char* fileName, char* buffer) {
    assert(fileName != NULL);
    assert(buffer != NULL);

    system("mkdir -p latexPdfs");
    const size_t TMP_BUFF_SIZE = 100;
    char tmp[TMP_BUFF_SIZE] = {};
    snprintf(tmp, TMP_BUFF_SIZE, "cd ./latexPdfs; xelatex %s > logErrors; cd ..", fileName);
    system(tmp);
}

ArifmTreeErrors saveArifmTreeToFile(ArifmTree* tree, const char* fileName) {
    IF_ARG_NULL_RETURN(tree);

    const size_t TMP_BUFF_SIZE = 100;
    char tmp[TMP_BUFF_SIZE] = {};
    snprintf(tmp, TMP_BUFF_SIZE, "latexPdfs/%s", fileName);
    FILE* file = fopen(tmp, "w");
    IF_NOT_COND_RETURN(file != NULL, ARIFM_TREE_FILE_OPENING_ERROR);

    char* buffer = NULL;
    IF_ERR_RETURN(recursiveTreeSaveToFile(tree, tree->root, 0, &buffer));
    LOG_DEBUG_VARS(buffer);

    fprintf(file,
        "\\documentclass[12pt,a4paper]{extreport}\n"
        "\\begin{document}\n"
        "$%s$\n"
        "\\end{document}\n",
    buffer);
    fclose(file);
    FREE(buffer);

    saveTexBufferToFile(fileName, buffer);

    return ARIFM_TREE_STATUS_OK;
}
