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
        size_t res = getCopyNodeInd(tree, destTree, old.left);
        node = &destTree->memBuff[dest];
        node->left = res;
    }
    if (old.right != 0) {
        size_t res = getCopyNodeInd(tree, destTree, old.right);
        node = &destTree->memBuff[dest];
        node->right = res;
    }

    return dest;
}

#define COPY(srcNodeInd) getCopyNodeInd(tree, destTree, srcNodeInd)

#define NEW_FUNC_NODE(intData, left, right) \
    constructNodeWithKidsNoErrors(destTree, FUNC, {.data = intData}, left, right)

#define NEW_NUM_NODE(data) \
    constructNodeWithKidsNoErrors(destTree, NUM, {.doubleData = data}, 0, 0)

#define NEW_VAR_NODE(intData) \
    constructNodeWithKidsNoErrors(destTree, VAR, {.data = intData}, 0, 0)

#define DIFF(node) \
    findDerivativeOfNode(tree, node, destTree)

#define FUNC ARIFM_TREE_FUNC_NODE
#define NUM  ARIFM_TREE_NUMBER_NODE
#define VAR  ARIFM_TREE_VAR_NODE

#define ADD(left, right)                                                        \
    do {                                                                        \
        return NEW_FUNC_NODE(ELEM_FUNC_ADD, DIFF(left), DIFF(right));           \
    } while (0)

#define SUB(left, right)                                                        \
    do {                                                                        \
        return NEW_FUNC_NODE(ELEM_FUNC_SUB, DIFF(left), DIFF(right));           \
    } while (0)

#define MUL(left, right)                                                        \
    do {                                                                        \
        return  NEW_FUNC_NODE(ELEM_FUNC_ADD,                                    \
                    NEW_FUNC_NODE(ELEM_FUNC_MUL, DIFF(left), COPY(right)),      \
                    NEW_FUNC_NODE(ELEM_FUNC_MUL, COPY(left), DIFF(right))       \
                );                                                              \
    } while (0)

#define DIV

#define DIV(left, right)                                                        \
    do {                                                                        \
        return  NEW_FUNC_NODE(ELEM_FUNC_DIV,                                    \
                    NEW_FUNC_NODE(ELEM_FUNC_SUB,                                \
                        NEW_FUNC_NODE(ELEM_FUNC_MUL, DIFF(left), COPY(right)),  \
                        NEW_FUNC_NODE(ELEM_FUNC_MUL, COPY(left), DIFF(right))   \
                    ),                                                          \
                    NEW_FUNC_NODE(ELEM_FUNC_POW,                                \
                        COPY(right),                                            \
                        NEW_NUM_NODE(2)                                        \
                    )                                                          \
                );                                                              \
    } while (0)

#define POW
#define LOG
#define ROOT

#define COS(left, right)                                                        \
    do {                                                                        \
        return NEW_FUNC_NODE(ELEM_FUNC_MUL,                                     \
            NEW_NUM_NODE(-1),                                                   \
            NEW_FUNC_NODE(ELEM_FUNC_MUL,                                        \
                NEW_FUNC_NODE(ELEM_FUNC_SIN, 0, COPY(right)),                   \
                DIFF(right)                                                     \
            )                                                                   \
        );                                                                      \
    } while (0)

#define SIN
//#define CTAN

// #define SIN(left, right)                                                        \
//     do {                                                                        \
//         return NEW_FUNC_NODE(ELEM_FUNC_COS, 0, DIFF(right));                    \
//     } while (0)

#define TAN(left, right)                                                        \
    do {                                                                        \
        return NEW_FUNC_NODE(ELEM_FUNC_DIV,                                     \
            DIFF(right),                                                    \
            NEW_FUNC_NODE(ELEM_FUNC_POW,                                        \
                NEW_FUNC_NODE(ELEM_FUNC_COS, 0, COPY(right)),                   \
                NEW_NUM_NODE(2)                                                 \
            )                                                                   \
        );                                                                      \
    } while (0)

#define CTAN(left, right)                                                       \
    do {                                                                        \
        return NEW_FUNC_NODE(ELEM_FUNC_MUL,                                     \
            NEW_NUM_NODE(-1),                                                   \
            NEW_FUNC_NODE(ELEM_FUNC_MUL,                                        \
                DIFF(right),                                                    \
                NEW_FUNC_NODE(ELEM_FUNC_DIV,                                    \
                    NEW_NUM_NODE(1),                                            \
                    NEW_FUNC_NODE(ELEM_FUNC_POW,                                \
                        NEW_FUNC_NODE(ELEM_FUNC_SIN, 0, DIFF(right)),           \
                        NEW_NUM_NODE(2)                                         \
                    )                                                           \
                )                                                               \
            )                                                                   \
        );                                                                      \
    } while (0)

#endif
