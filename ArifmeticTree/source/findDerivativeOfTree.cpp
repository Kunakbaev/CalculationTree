#include <ctype.h>

#include "../include/commonFileStart.hpp"
#include "../../ArifmOperations/include/functionsDiffDefines.hpp"

static size_t findDerivativeOfNode(const ArifmTree* tree, size_t nodeInd, ArifmTree* destTree) {
    assert(tree     != NULL);
    assert(destTree != NULL);
    assert(nodeInd  <= tree->freeNodeIndex);

    Node node = tree->memBuff[nodeInd];
    if (node.nodeType == ARIFM_TREE_NUMBER_NODE) {
        return NEW_NUM_NODE(0);
    }
    if (node.nodeType == ARIFM_TREE_VAR_NODE) {
        return NEW_NUM_NODE(1);
    }

    LOG_DEBUG_VARS(nodeInd, node.left, node.right);

#define ARIFM_OPP_GENERAL_FUNC(_, __, enumName, ___)    \
    case ELEM_FUNC_##enumName:                          \
        enumName(node.left, node.right);                                        \
        break;

    switch (node.data) {
        #include "../../ArifmOperations/include/functionsCodeGen/allFuncs.hpp"
        default:
            LOG_ERROR("unknown shit, node->data bruh");
            return -1;
    }
#undef  ARIFM_OPP_GENERAL_FUNC

    return 0;
}

#include "../include/assingParents.hpp"

ArifmTreeErrors getDerivativeOfTree(ArifmTree* source, ArifmTree* dest) {
    IF_ARG_NULL_RETURN(source);
    IF_ARG_NULL_RETURN(dest);

    IF_ERR_RETURN(constructArifmTree(dest, &source->dumper));
    dest->root = findDerivativeOfNode(source, source->root, dest);
    IF_ERR_RETURN(assignParentNodes(dest, dest->root, 0));

    char* output = (char*)calloc(2000, sizeof(char));
    assert(output != NULL);
    char* ptr = output;
    Node2stringSettings nodeDumpSettings = {
        false, false, false
    };
    IF_ERR_RETURN(dumpArifmTreeInConsole(dest, dest->root, &ptr, &nodeDumpSettings));
    LOG_DEBUG_VARS(output);
    FREE(output);

    IF_ERR_RETURN(openImageOfCurrentStateArifmTree(dest));

    return ARIFM_TREE_STATUS_OK;
}
