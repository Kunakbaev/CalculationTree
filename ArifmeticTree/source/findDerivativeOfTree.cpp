#include <ctype.h>

#include "../include/commonFileStart.hpp"
#include "../../ArifmOperations/include/functionsDiffDefines.hpp"

static /* FIXME: ssize_t*/ size_t findDerivativeOfNode(const ArifmTree* tree, size_t nodeInd, ArifmTree* destTree) {
    //LOG_DEBUG_VARS(tree, tree->memBuffSize, nodeInd);
    assert(tree     != NULL);
    assert(destTree != NULL);
    assert(nodeInd  <= tree->freeNodeIndex);

    // if (tree.error) return;
    if (!nodeInd) return -1;

    LOG_DEBUG_VARS(nodeInd);
    Node node = *getArifmTreeNodePtr(tree, nodeInd);
    LOG_DEBUG_VARS(node.nodeType == ARIFM_TREE_FUNC_NODE, node.data, node.doubleData, node.left, node.right);
    switch (node.nodeType) {
        case ARIFM_TREE_NUMBER_NODE:
            return NEW_NUM_NODE(0);
        case ARIFM_TREE_VAR_NODE:
            return NEW_NUM_NODE(1);
        case ARIFM_TREE_FUNC_NODE:
            #define ARIFM_OPP_GENERAL_FUNC(_, __, enumName, ...)                        \
                case ELEM_FUNC_##enumName:                                              \
                    enumName(node.left, node.right);                                    \
                    break;

            switch (node.data) {
                #include "../../ArifmOperations/include/functionsCodeGen/allFuncs.hpp"
                default:
                    LOG_ERROR("unknown shit, node->data bruh");
                    return -1;
            }
            #undef  ARIFM_OPP_GENERAL_FUNC
        default:
            LOG_ERROR(getArifmTreeErrorMessage(ARIFM_TREE_INVALID_NODE_TYPE));
            return ARIFM_TREE_INVALID_NODE_TYPE;
    }
    //LOG_DEBUG_VARS(nodeInd, node.left, node.right);

    return 0;
}

#include "../include/assingParents.hpp"

//                         FIXME: const
ArifmTreeErrors getDerivativeOfTree(const ArifmTree* source, ArifmTree* dest) {
    IF_ARG_NULL_RETURN(source);
    IF_ARG_NULL_RETURN(dest);

    IF_ERR_RETURN(getCopyOfTree(source, dest));
    IF_ERR_RETURN(simplifyTree(dest));

    dest->root = findDerivativeOfNode(source, source->root, dest);
    IF_ERR_RETURN(assignParentNodes(dest, dest->root, 0));

    IF_ERR_RETURN(simplifyTree(dest));
    //IF_ERR_RETURN(dumpArifmTreeInConsole(dest));
    //IF_ERR_RETURN(openImageOfCurrentStateArifmTree(dest));
    // ------------------------------------------------------------------------

    return ARIFM_TREE_STATUS_OK;
}
