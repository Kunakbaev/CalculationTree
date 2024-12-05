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


#include "../../ArifmOperations/include/simplificationFuncs.hpp"

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
        LOG_ERROR("-------- var ------------------");
        return ARIFM_TREE_STATUS_OK;
    }

    IF_ERR_RETURN(simplifyTreeRecursive(tree, node.left,  &newLeftNode,  &wasVarInLeft,  wasSimplification));
    IF_ERR_RETURN(simplifyTreeRecursive(tree, node.right, &newRightNode, &wasVarInRight, wasSimplification));

    Function func = {};
    ARIFM_OPS_ERR_CHECK(getFuncByIndex(node.data, &func));
    double one = 0;
    double two = 0;
    if (newLeftNode)
        one = getArifmTreeNodePtr(tree, newLeftNode) ->doubleData;
    if (newRightNode)
        two = getArifmTreeNodePtr(tree, newRightNode)->doubleData;

    *resultNodeInd = nodeInd;

    #define ARIFM_OPP_GENERAL_FUNC(_, __, name, ...)     \
        case ELEM_FUNC_##name:                           \
            name##_SIMPLIFY;                             \
            break;

    TRY_REDUCE_TO_CONST_RET_IF_OK;
    switch (func.name) {
        #include "../../ArifmOperations/include/functionsCodeGen/allFuncs.hpp"
        default:
            LOG_ERROR("unknown func name");
            return ARIFM_TREE_INVALID_ARGUMENT; // TODO: add appropriate error
    }
    #undef ARIFM_OPP_GENERAL_FUNC

    *wasVariable = wasVarInLeft | wasVarInRight;
    *resultNodeInd = constructNodeWithKidsNoErrors(tree, node.nodeType,
                                                   {.data=node.data}, newLeftNode, newRightNode);
    getArifmTreeNodePtr(tree, *resultNodeInd)->doubleData = node.doubleData;
    tree->memBuff[node.memBuffIndex].nodeType = ARIFM_TREE_INVALID_NODE;

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

