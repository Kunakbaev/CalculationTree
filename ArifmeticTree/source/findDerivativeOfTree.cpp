#include <ctype.h>

#include "../include/commonFileStart.hpp"
#include "../../ArifmOperations/include/functionsDiffDefines.hpp"

static /* FIXME: ssize_t*/ size_t findDerivativeOfNode(const ArifmTree* tree, size_t nodeInd, ArifmTree* destTree) {
    assert(tree     != NULL);
    assert(destTree != NULL);
    assert(nodeInd  <= tree->freeNodeIndex);

    // if (tree.error) return;
    if (!nodeInd) return -1;

    Node node = *getArifmTreeNodePtr(tree, nodeInd); // FIXME: getNode(&tree, nodeInd);
    if (node.nodeType == ARIFM_TREE_NUMBER_NODE) { // if (!tryDeriveNumberNode ||
                                                   //     !tryerror:DeriveVarNode ||
                                                   //     !tryDeriveOperation) {error}
        return NEW_NUM_NODE(0);
    }
    if (node.nodeType == ARIFM_TREE_VAR_NODE) {
        return NEW_NUM_NODE(1);
    }

    // FIXME: rewrite to switch
    LOG_DEBUG_VARS(nodeInd, node.left, node.right);

#define ARIFM_OPP_GENERAL_FUNC(_, __, enumName, ...)    \
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

//                         FIXME: const
ArifmTreeErrors getDerivativeOfTree(const ArifmTree* source, ArifmTree* dest) {
    IF_ARG_NULL_RETURN(source);
    IF_ARG_NULL_RETURN(dest);

    // FIXME: simplify tree first
    //IF_ERR_RETURN(simplifyTree(source));

    IF_ERR_RETURN(constructArifmTree(dest, &source->dumper));
    dest->root = findDerivativeOfNode(source, source->root, dest);
    IF_ERR_RETURN(assignParentNodes(dest, dest->root, 0));

    // ------------------------------------------------------------------------ // FIXME: в функцию!
    Node2stringSettings nodeDumpSettings = {
        .isNodeTypeNeeded = false,
        .isFuncTypeNeeded = false,
        .isBracketsNeeded = false,
    };
    char* output = (char*)calloc(200000, sizeof(char));
    assert(output != NULL);
    char* ptr = output;
    IF_ERR_RETURN(dumpArifmTreeInConsole(dest, dest->root, &ptr, &nodeDumpSettings));
    FREE(output);
    LOG_DEBUG_VARS(output);
    IF_ERR_RETURN(openImageOfCurrentStateArifmTree(dest));
    // ------------------------------------------------------------------------

    return ARIFM_TREE_STATUS_OK;
}
