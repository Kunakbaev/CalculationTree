#include <ctype.h>

#include "../include/commonFileStart.hpp"
#include "../include/assingParents.hpp"

static ArifmTreeErrors simplifyNoVarsSubtree(ArifmTree* tree, size_t newLeftNode, size_t newRightNode,
                                             const Function* func, size_t* resultNodeInd) {
    double one = tree->memBuff[newLeftNode]. doubleData;
    double two = tree->memBuff[newRightNode].doubleData;
    double calcRes = (*func->calculationFunc)(one, two);
    LOG_DEBUG_VARS(one, two, calcRes, func->name);

    *resultNodeInd = constructNodeWithKidsNoErrors(tree, ARIFM_TREE_NUMBER_NODE,
                                                    {.doubleData = calcRes}, 0, 0);
    return ARIFM_TREE_STATUS_OK;
}
//
// static ArifmTreeErrors simplificationWithRightSonLeft(ArifmTree* tree, size_t* resultNodeInd, bool* wasVariable,
//                                                       bool wasVarInLeft, double one, size_t newLeftNode,
//                                                       bool wasVarInRight, double two, size_t newRightNode,
//                                                       FunctionsNames funcName) {
//     *wasVariable = wasVarInRight;
//     *resultNodeInd = newLeftNode;
//
//     if (funcName == ELEM_FUNC_POW) {
//
//     }
// }

static ArifmTreeErrors simplifyTreeRecursive(ArifmTree* tree, size_t nodeInd, size_t* resultNodeInd, bool* wasVariable) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(wasVariable);
    IF_ARG_NULL_RETURN(resultNodeInd);
    IF_NOT_COND_RETURN(nodeInd <= tree->freeNodeIndex,
                       ARIFM_TREE_INVALID_ARGUMENT);

    Node node = tree->memBuff[nodeInd];
    size_t newLeftNode   = 0;
    size_t newRightNode  = 0;
    bool   wasVarInLeft  = false;
    bool   wasVarInRight = false;

    if (node.nodeType != ARIFM_TREE_FUNC_NODE) {
        *resultNodeInd = nodeInd;
        *wasVariable   = node.nodeType == ARIFM_TREE_VAR_NODE;
        return ARIFM_TREE_STATUS_OK;
    }

    if (node.left != 0) {
        IF_ERR_RETURN(simplifyTreeRecursive(tree, node.left,  &newLeftNode,  &wasVarInLeft));
    }
    if (node.right != 0) {
        IF_ERR_RETURN(simplifyTreeRecursive(tree, node.right, &newRightNode, &wasVarInRight));
    }

    Function func = {};
    ARIFM_OPS_ERR_CHECK(getFuncByIndex(node.data, &func));
    double one = tree->memBuff[newLeftNode]. doubleData;
    double two = tree->memBuff[newRightNode].doubleData;

    *wasVariable = wasVarInLeft | wasVarInRight;
    if (!wasVarInLeft && !wasVarInRight) {
        double calcRes = (*func.calculationFunc)(one, two);
        LOG_DEBUG_VARS(one, two, calcRes, func.name);

        *resultNodeInd = constructNodeWithKidsNoErrors(tree, ARIFM_TREE_NUMBER_NODE,
                                                       {.doubleData = calcRes}, 0, 0);
        return ARIFM_TREE_STATUS_OK;
    }

    *resultNodeInd = nodeInd;
    if (func.name == ELEM_FUNC_ADD || func.name == ELEM_FUNC_SUB) {
        if (one == 0 && !wasVarInLeft) {
            *resultNodeInd = newRightNode;
            return ARIFM_TREE_STATUS_OK;
        }
        if (two == 0 && !wasVarInRight) {
            *resultNodeInd = newLeftNode;
            return ARIFM_TREE_STATUS_OK;
        }
    }

    if (func.name == ELEM_FUNC_MUL) {
        if (one == 1 && !wasVarInLeft) {
            *wasVariable = wasVarInLeft;
            *resultNodeInd = newRightNode;
            return ARIFM_TREE_STATUS_OK;
        }
        if (two == 1 && !wasVarInRight) {
            *wasVariable = wasVarInRight;
            *resultNodeInd = newLeftNode;
            return ARIFM_TREE_STATUS_OK;
        }
        if ((one == 0 && !wasVarInLeft) ||
            (two == 0 && !wasVarInRight)) {
            LOG_ERROR("--------------fdas------------------");
            *wasVariable = false;
            *resultNodeInd = constructNodeWithKidsNoErrors(tree, ARIFM_TREE_NUMBER_NODE,
                                                           {.doubleData = 0}, 0, 0);
            return ARIFM_TREE_STATUS_OK;
        }
        return ARIFM_TREE_STATUS_OK;
    }

    if (func.name == ELEM_FUNC_DIV) {
        if (two == 1 && !wasVarInRight) {
            *wasVariable = wasVarInLeft;
            *resultNodeInd = newLeftNode;
            return ARIFM_TREE_STATUS_OK;
        }
        if (one == 0 && !wasVarInLeft) {
            *wasVariable = false;
            *resultNodeInd = constructNodeWithKidsNoErrors(tree, ARIFM_TREE_NUMBER_NODE,
                                                           {.doubleData = 0}, 0, 0);
            return ARIFM_TREE_STATUS_OK;
        }
        if (two == 0 && !wasVarInRight) {
            // TODO: division error

        }
    }

    if (func.name == ELEM_FUNC_POW) {
        if ((one == 1 && !wasVarInLeft) ||
            (two == 0 && !wasVarInRight)) {
            *wasVariable = false;
            *resultNodeInd = constructNodeWithKidsNoErrors(tree, ARIFM_TREE_NUMBER_NODE,
                                                           {.doubleData = 1}, 0, 0);
            return ARIFM_TREE_STATUS_OK;
        }
        if (two == 1 && !wasVarInRight) {
            *wasVariable = wasVarInLeft;
            *resultNodeInd = newLeftNode;
            return ARIFM_TREE_STATUS_OK;
        }
        if (one == 0 && !wasVarInLeft) {
            *wasVariable = wasVarInLeft;
            *resultNodeInd = constructNodeWithKidsNoErrors(tree, ARIFM_TREE_NUMBER_NODE,
                                                           {.doubleData = 0}, 0, 0);
            return ARIFM_TREE_STATUS_OK;
        }
    }

    *resultNodeInd = constructNodeWithKidsNoErrors(tree, node.nodeType,
                                                   {.data=node.data}, newLeftNode, newRightNode);
    tree->memBuff[*resultNodeInd].doubleData = node.doubleData;

    return ARIFM_TREE_STATUS_OK;
}

ArifmTreeErrors simplifyTree(ArifmTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    bool wasChange = true;
    while (wasChange) {
        bool wasVar = false;
        size_t newRoot = 0;
        IF_ERR_RETURN(simplifyTreeRecursive(tree, tree->root, &newRoot, &wasVar));
        wasChange &= newRoot != tree->root;
        tree->root = newRoot;
        LOG_DEBUG_VARS("bruh");
        break;
    }

    IF_ERR_RETURN(assignParentNodes(tree, tree->root, 0));

    return ARIFM_TREE_STATUS_OK;
}

