#include <ctype.h>

#include "../include/arifmOperations.hpp"

void initArifmTreeNodeWithString(Node* node, const char* line) {
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


