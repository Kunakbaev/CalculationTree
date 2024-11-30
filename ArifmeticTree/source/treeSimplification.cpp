#include <ctype.h>

#include "../include/commonFileStart.hpp"
#include "../include/assingParents.hpp"

static ArifmTreeErrors simplifyNoVarsSubtree(ArifmTree* tree, size_t newLeftNode, size_t newRightNode,
                                             const Function* func, size_t* resultNodeInd) {
    double one = getArifmTreeNodePtr(tree,  newLeftNode)->doubleData;
    double two = getArifmTreeNodePtr(tree, newRightNode)->doubleData;
    double calcRes = (*func->calculationFunc)(one, two);
    LOG_DEBUG_VARS(one, two, calcRes, func->name);

    *resultNodeInd = constructNodeWithKidsNoErrors(tree, ARIFM_TREE_NUMBER_NODE,
                                                    {.doubleData = calcRes}, 0, 0);
    return ARIFM_TREE_STATUS_OK;
}

static ArifmTreeErrors simplifyTreeRecursive(ArifmTree* tree, size_t nodeInd, size_t* resultNodeInd,
                                             bool* wasVariable, bool* wasSimplification) {
    if (!nodeInd)
        return ARIFM_TREE_STATUS_OK;

    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(wasVariable);
    IF_ARG_NULL_RETURN(wasSimplification);
    IF_ARG_NULL_RETURN(resultNodeInd);

    Node node = *getArifmTreeNodePtr(tree, nodeInd);
    size_t newLeftNode   = 0;
    size_t newRightNode  = 0;
    bool   wasVarInLeft  = false;
    bool   wasVarInRight = false;

    if (node.nodeType != ARIFM_TREE_FUNC_NODE) {
        *resultNodeInd = nodeInd;
        *wasVariable   = node.nodeType == ARIFM_TREE_VAR_NODE;
        return ARIFM_TREE_STATUS_OK;
    }

    IF_ERR_RETURN(simplifyTreeRecursive(tree, node.left,  &newLeftNode,  &wasVarInLeft,  wasSimplification));
    IF_ERR_RETURN(simplifyTreeRecursive(tree, node.right, &newRightNode, &wasVarInRight, wasSimplification));

    Function func = {};
    ARIFM_OPS_ERR_CHECK(getFuncByIndex(node.data, &func));
    double one = getArifmTreeNodePtr(tree, newLeftNode) ->doubleData;
    double two = getArifmTreeNodePtr(tree, newRightNode)->doubleData;

    // FIXME: move to arifm ops/simplificatorFuncs.hpp
    #define REDUCE_TO_CONST_AND_RETURN(number)                                                  \
        do {                                                                                    \
            *wasSimplification = true;                                                          \
            *wasVariable       = false;                                                         \
            *resultNodeInd     = constructNodeWithKidsNoErrors(tree, ARIFM_TREE_NUMBER_NODE,    \
                                                           {.doubleData = number}, 0, 0);       \
            return ARIFM_TREE_STATUS_OK;                                                        \
        } while (0)                                                                             \

    #define LEAVE_LEFT_SON_AND_RETURN()                                                         \
        do {                                                                                    \
            *wasSimplification = true;                                                          \
            *wasVariable       = wasVarInLeft;                                                  \
            *resultNodeInd     = newLeftNode;                                                   \
            return ARIFM_TREE_STATUS_OK;                                                        \
        } while (0)

    #define LEAVE_RIGHT_SON_AND_RETURN()                                                        \
        do {                                                                                    \
            *wasSimplification = true;                                                          \
            *wasVariable   = wasVarInRight;                                                     \
            *resultNodeInd = newRightNode;                                                      \
            return ARIFM_TREE_STATUS_OK;                                                        \
        } while (0)

    #define IS_L_EQ2(num) (one == (num) && !wasVarInLeft)
    #define IS_R_EQ2(num) (two == (num) && !wasVarInRight)

    if (!wasVarInLeft && !wasVarInRight) {
        double calcRes = (*func.calculationFunc)(one, two);
        LOG_DEBUG_VARS(one, two, calcRes, func.name);
        REDUCE_TO_CONST_AND_RETURN(calcRes);
    }

    *resultNodeInd = nodeInd;
    if (func.name == ELEM_FUNC_ADD || func.name == ELEM_FUNC_SUB) {
        if (IS_L_EQ2(0)) LEAVE_RIGHT_SON_AND_RETURN();
        if (IS_R_EQ2(0)) LEAVE_LEFT_SON_AND_RETURN();
    }

    if (func.name == ELEM_FUNC_MUL) {
        if (IS_L_EQ2(1))                LEAVE_RIGHT_SON_AND_RETURN();
        if (IS_R_EQ2(1))                LEAVE_LEFT_SON_AND_RETURN();
        if (IS_L_EQ2(0) || IS_R_EQ2(0)) REDUCE_TO_CONST_AND_RETURN(0);
    }

    if (func.name == ELEM_FUNC_DIV) {
        if (IS_R_EQ2(1)) LEAVE_LEFT_SON_AND_RETURN();
        if (IS_L_EQ2(0)) REDUCE_TO_CONST_AND_RETURN(0);
        if (IS_R_EQ2(0)) {
            // TODO: division error
        }
    }

    if (func.name == ELEM_FUNC_POW) {
        if (IS_L_EQ2(1) || IS_R_EQ2(0)) REDUCE_TO_CONST_AND_RETURN(1);
        if (IS_R_EQ2(1))                LEAVE_LEFT_SON_AND_RETURN();
        if (IS_L_EQ2(0))                REDUCE_TO_CONST_AND_RETURN(0);
    }

    *wasVariable = wasVarInLeft | wasVarInRight;
    *resultNodeInd = constructNodeWithKidsNoErrors(tree, node.nodeType,
                                                   {.data=node.data}, newLeftNode, newRightNode);
    getArifmTreeNodePtr(tree, *resultNodeInd)->doubleData = node.doubleData;

    return ARIFM_TREE_STATUS_OK;
}

ArifmTreeErrors simplifyTree(ArifmTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    bool wasSimplification = true;
    while (wasSimplification) {
        bool wasVar = false;
        size_t newRoot = 0;
        wasSimplification = false;
        IF_ERR_RETURN(simplifyTreeRecursive(tree, tree->root, &newRoot, &wasVar, &wasSimplification));
        tree->root = newRoot;
    }

    IF_ERR_RETURN(assignParentNodes(tree, tree->root, 0));

    return ARIFM_TREE_STATUS_OK;
}

