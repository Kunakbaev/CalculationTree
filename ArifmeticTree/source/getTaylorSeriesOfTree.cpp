#include <ctype.h>

#include "../include/commonFileStart.hpp"
#include "../include/assingParents.hpp"

const size_t MAX_DERIVATIVE_ORDER = 30;

ArifmTreeErrors getNthDerivativeOfTree(const ArifmTree* tree, ArifmTree* result, int order) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(result);
    IF_NOT_COND_RETURN(order < MAX_DERIVATIVE_ORDER, ARIFM_TREE_INVALID_ARGUMENT);

    IF_ERR_RETURN(getCopyOfTree(tree, result));
    //IF_ERR_RETURN(openImageOfCurrentStateArifmTree(result));
    ArifmTree copy = {};
    while (order--) {
        // does it work?
        IF_ERR_RETURN(getCopyOfTree(result, &copy));
        IF_ERR_RETURN(destructArifmTree(result));
        IF_ERR_RETURN(getDerivativeOfTree(&copy, result));
        IF_ERR_RETURN(simplifyTree(result));
        //IF_ERR_RETURN(openImageOfCurrentStateArifmTree(result));
        destructArifmTree(&copy);
    }

    return ARIFM_TREE_STATUS_OK;
}

#include "../../ArifmOperations/include/functionsDiffDefines.hpp"

ArifmTreeErrors substituteConstRecursive(ArifmTree* tree, size_t curNodeInd, double point) {
    IF_ARG_NULL_RETURN(tree);

    if (!curNodeInd)
        return ARIFM_TREE_STATUS_OK;

    Node* node = getArifmTreeNodePtr(tree, curNodeInd);
    if (node->nodeType == ARIFM_TREE_VAR_NODE) {
        node->nodeType   = ARIFM_TREE_NUMBER_NODE;
        node->doubleData = point;
        return ARIFM_TREE_STATUS_OK;
    }

    IF_ERR_RETURN(substituteConstRecursive(tree, node->left , point));
    IF_ERR_RETURN(substituteConstRecursive(tree, node->right, point));

    return ARIFM_TREE_STATUS_OK;
}

ArifmTreeErrors substituteConst(ArifmTree* tree, double point) {
    IF_ARG_NULL_RETURN(tree);

    substituteConstRecursive(tree, tree->root, point);

    return ARIFM_TREE_STATUS_OK;
}

ArifmTreeErrors getTaylorSeriesOfTree(const ArifmTree* tree, ArifmTree* destTree,
                                      size_t order, size_t variable, double point) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(destTree);

    double fact = 1;
    IF_ERR_RETURN(constructArifmTree(destTree, tree->dumper));
    for (size_t i = 0; i <= order; ++i, fact *= i) {
        ArifmTree add = {};
        IF_ERR_RETURN(getNthDerivativeOfTree(tree, &add, i));
        size_t prevNode = NEW_NUM_NODE(0);
        if (i) prevNode = destTree->root;
        IF_ERR_RETURN(substituteConst(&add, point));
        //openImageOfCurrentStateArifmTree(&add);

        destTree->root =
        NEW_ADD_NODE(
            prevNode,
            NEW_DIV_NODE(
                NEW_MUL_NODE(
                    getCopyOfSubtree(&add, destTree, add.root),
                    NEW_FUNC_NODE(ELEM_FUNC_POW,
                        NEW_SUB_NODE(
                            NEW_VAR_NODE(variable),
                            NEW_NUM_NODE(point)
                        ),
                        NEW_NUM_NODE((double)i)
                    )
                ),
                NEW_NUM_NODE(fact)
            )
        );
        destructArifmTree(&add);

        // WARNING: without this line tree is too big
        IF_ERR_RETURN(simplifyTree(destTree));
    }

    LOG_DEBUG_VARS(variable);
    LOG_ERROR("--------------------------------");
    // dumpArifmTree(destTree);
    openImageOfCurrentStateArifmTree(destTree);
    IF_ERR_RETURN(simplifyTree(destTree));
    LOG_DEBUG_VARS("simplified");
    openImageOfCurrentStateArifmTree(destTree);

    return ARIFM_TREE_STATUS_OK;
}
