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


#include "../../ArifmOperations/include/functionsDiffDefines.hpp"

size_t findDerivativeOfNode(const ArifmTree* tree, size_t nodeInd, ArifmTree* destTree) {
    assert(tree     != NULL);
    assert(destTree != NULL);
    assert(nodeInd  <= tree->freeNodeIndex);

    Node node = tree->memBuff[nodeInd];
    if (node.nodeType == ARIFM_TREE_NUMBER_NODE) {
        return NEW_NUM_NODE(0, 0, 0);
    }
    if (node.nodeType == ARIFM_TREE_VAR_NODE) {
        return NEW_NUM_NODE(1, 0, 0);
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

ArifmTreeErrors assignParentNodes(ArifmTree* tree, size_t nodeInd, size_t parentInd) {
    IF_ARG_NULL_RETURN(tree);
    IF_NOT_COND_RETURN(nodeInd <= tree->freeNodeIndex,
                       ARIFM_TREE_INVALID_ARGUMENT);

    if (!nodeInd)
        return ARIFM_TREE_STATUS_OK;

    Node* node = &tree->memBuff[nodeInd];
    LOG_DEBUG_VARS(nodeInd, node->memBuffIndex, node->left, node->right);
    node->parent = parentInd;
    if (node->left != 0) {
        IF_ERR_RETURN(assignParentNodes(tree, node->left, nodeInd));
    }
    if (node->right != 0) {
        IF_ERR_RETURN(assignParentNodes(tree, node->right, nodeInd));
    }

    return ARIFM_TREE_STATUS_OK;
}

ArifmTreeErrors getDerivativeOfTree(ArifmTree* source, ArifmTree* dest) {
    IF_ARG_NULL_RETURN(source);
    IF_ARG_NULL_RETURN(dest);

    IF_ERR_RETURN(constructArifmTree(dest, &source->dumper));
    dest->root = findDerivativeOfNode(source, source->root, dest);
    IF_ERR_RETURN(assignParentNodes(dest, dest->root, 0));

    char* output = (char*)calloc(2000, sizeof(char));
    assert(output != NULL);
    char* ptr = output;
    IF_ERR_RETURN(dumpArifmTreeInConsole(dest, dest->root, &ptr));
    LOG_DEBUG_VARS(output);
    FREE(output);

    IF_ERR_RETURN(openImageOfCurrentStateArifmTree(dest));

    return ARIFM_TREE_STATUS_OK;
}
