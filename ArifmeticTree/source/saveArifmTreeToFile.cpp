#include <ctype.h>

#include "../include/commonFileStart.hpp"

const size_t OUTPUT_BUFF_SIZE = 1 << 12;
char allOutputBuffer[OUTPUT_BUFF_SIZE] = {};
char* outputBuffPtr = allOutputBuffer;

static ArifmTreeErrors recursiveTreeSaveToFile(const ArifmTree* tree, size_t nodeInd, size_t parentInd, char** buffer) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(buffer);

    Node node = *getArifmTreeNodePtr(tree, nodeInd);

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
    // LOG_DEBUG_VARS(leftSonString);
    // LOG_DEBUG_VARS(rightSonString);
    if (strlen(leftSonString) != 0)
        FREE(leftSonString);
    if (strlen(rightSonString) != 0)
        FREE(rightSonString);
    // LOG_DEBUG_VARS(*buffer);

    if (parentInd == 0)
        return ARIFM_TREE_STATUS_OK;

    Node parent = *getArifmTreeNodePtr(tree, parentInd);
    bool isLeftSon = nodeInd == parent.left;
    if (isNeedForBrackets(&parent, &node, isLeftSon)) {
        const char* leftBracket  = "\\left(";
        const char* rightBracket = "\\right)";
        char* tmp = (char*)calloc(strlen(*buffer) +
                                    strlen(leftBracket) + strlen(rightBracket) + 1, sizeof(char));
        IF_NOT_COND_RETURN(tmp != NULL, ARIFM_TREE_MEMORY_ALLOCATION_ERROR);

        sprintf(tmp, "%s%s%s", leftBracket, *buffer, rightBracket);
        FREE(*buffer);
        *buffer = tmp;
        //LOG_DEBUG_VARS(*buffer);
    }

    return ARIFM_TREE_STATUS_OK;
}

static ArifmTreeErrors renderTexFile(const char* fileName) {
    IF_ARG_NULL_RETURN(fileName);

    system("mkdir -p latexPdfs");

    const size_t TMP_BUFF_SIZE = 300;
    char tmp[TMP_BUFF_SIZE] = {};
    snprintf(tmp, TMP_BUFF_SIZE, "latexPdfs/%s", fileName);
    FILE* file = fopen(tmp, "w"); // WARNING: it's bad that file is every time rewritten from scratch
    IF_NOT_COND_RETURN(file != NULL, ARIFM_TREE_FILE_OPENING_ERROR);
    LOG_DEBUG_VARS(tmp, allOutputBuffer);
    fprintf(file,
        "\\documentclass[12pt,a4paper]{extreport}\n"
        "\\begin{document}\n"
        "$\n%s$\n"
        "\\end{document}",
        allOutputBuffer
    );
    IF_NOT_COND_RETURN(file != NULL, ARIFM_TREE_FILE_OPENING_ERROR);
    fclose(file);

    snprintf(tmp, TMP_BUFF_SIZE, "cd ./latexPdfs; xelatex %s > logErrors; cd ..", fileName);
    system(tmp);

    return ARIFM_TREE_STATUS_OK;
}

ArifmTreeErrors addTreeLatexRepresenationToFile(const ArifmTree* tree, const char* fileName) {
    IF_ARG_NULL_RETURN(tree);

    char* buffer = NULL;
    IF_ERR_RETURN(recursiveTreeSaveToFile(tree, tree->root, 0, &buffer));
    //LOG_DEBUG_VARS(buffer);

    outputBuffPtr += snprintf(outputBuffPtr, OUTPUT_BUFF_SIZE - (outputBuffPtr - allOutputBuffer),
                                "\\\\\n%s\n", buffer);
    FREE(buffer);
    LOG_DEBUG_VARS(allOutputBuffer);

    renderTexFile(fileName);

    return ARIFM_TREE_STATUS_OK;
}
