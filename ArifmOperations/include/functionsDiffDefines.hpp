#ifndef ARIFM_OPERATIONS_INCLUDE_FUNCTIONS_DIFF_DEFINES_HPP
#define ARIFM_OPERATIONS_INCLUDE_FUNCTIONS_DIFF_DEFINES_HPP

static size_t getCopyNodeInd(const ArifmTree* tree, ArifmTree* destTree, size_t srcNodeInd) {
    size_t dest = 0;
    IF_ERR_RETURN(getNewNode(destTree, &dest));
    Node* node = &destTree->memBuff[dest];
    Node   old = tree->memBuff[srcNodeInd];

    node->nodeType   = old.nodeType;
    node->data       = old.data;
    node->doubleData = old.doubleData;

    if (dest == 2) {
        LOG_DEBUG_VARS(old.left, old.right);
    }
    if (old.left != 0) {
        node->left = getCopyNodeInd(tree, destTree, old.left);
    }
    if (old.right != 0) {
        node->right = getCopyNodeInd(tree, destTree, old.right);
    }

    return dest;
}

#define COPY(srcNodeInd) getCopyNodeInd(tree, destTree, srcNodeInd)

#define NEW_FUNC_NODE(intData, left, right) \
    constructNodeWithKidsNoErrors(destTree, FUNC, {.data = intData}, left, right)

#define NEW_NUM_NODE(data, left, right) \
    constructNodeWithKidsNoErrors(destTree, NUM, {.doubleData = data}, left, right)

#define NEW_VAR_NODE(intData, left, right) \
    constructNodeWithKidsNoErrors(destTree, VAR, {.data = intData}, left, right)

#define DIFF(node) \
    findDerivativeOfNode(tree, node, destTree)

#define FUNC ARIFM_TREE_FUNC_NODE
#define NUM  ARIFM_TREE_NUMBER_NODE
#define VAR  ARIFM_TREE_VAR_NODE

#define ADD(left, right) \
    do {    \
        return NEW_FUNC_NODE(ELEM_FUNC_ADD, DIFF(left), DIFF(right)); \
    } while (0)

#define DIV

#define MUL(left, right)                                                        \
    do {                                                                        \
        return  NEW_FUNC_NODE(ELEM_FUNC_ADD,                                   \
                    NEW_FUNC_NODE(ELEM_FUNC_MUL, DIFF(left), COPY(right)),     \
                    NEW_FUNC_NODE(ELEM_FUNC_MUL, COPY(left), DIFF(right))     \
                );                                                              \
    } while (0)

// #define DIV(left, right)                                                        \
//     do {                                                                        \
//         return  NEW_NODE(FUNC, ELEM_FUNC_DIV,                                   \
//                     NEW_NODE(FUNC, ELEM_FUNC_SUB,                               \
//                         NEW_NODE(FUNC, ELEM_FUNC_MUL, DIFF(left), COPY(right)), \
//                         NEW_NODE(FUNC, ELEM_FUNC_MUL, COPY(left), DIFF(right))  \
//                     ),                                                          \
//                     NEW_NODE(FUNC, ELEM_FUNC_POW,                               \
//                         COPY(right),                                            \
//                         NEW_NODE(NUM,  2, 0, 0),                                \
//                     ),                                                          \
//                 );                                                              \
//     } while (0)

#define SUB
#define POW
#define LOG
#define ROOT
#define COS
#define SIN
#define TAN
#define CTAN

#endif
