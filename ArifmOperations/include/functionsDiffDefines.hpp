#include <math.h>

#ifndef ARIFM_OPERATIONS_INCLUDE_FUNCTIONS_DIFF_DEFINES_HPP
#define ARIFM_OPERATIONS_INCLUDE_FUNCTIONS_DIFF_DEFINES_HPP

#define COPY(srcNodeInd) getCopyOfSubtree(tree, destTree, srcNodeInd)

#define NEW_NUM_NODE(data) \
    constructNodeWithKidsNoErrors(destTree, NUM, {.doubleData = data}, 0, 0)

#define NEW_VAR_NODE(intData) \
    constructNodeWithKidsNoErrors(destTree, VAR, {.data = intData}, 0, 0)

#define NEW_FUNC_NODE(intData, left, right) \
    constructNodeWithKidsNoErrors(destTree, FUNC, {.data = intData}, left, right)

static /* FIXME: ssize_t*/ size_t findDerivativeOfNode(const ArifmTree* tree, size_t nodeInd, ArifmTree* destTree);

#define DIFF(node) \
    findDerivativeOfNode(tree, node, destTree)

#define FUNC ARIFM_TREE_FUNC_NODE
#define NUM  ARIFM_TREE_NUMBER_NODE
#define VAR  ARIFM_TREE_VAR_NODE

#define NEW_ADD_NODE(...) NEW_FUNC_NODE(ELEM_FUNC_ADD, __VA_ARGS__)
#define NEW_SUB_NODE(...) NEW_FUNC_NODE(ELEM_FUNC_SUB, __VA_ARGS__)
#define NEW_MUL_NODE(...) NEW_FUNC_NODE(ELEM_FUNC_MUL, __VA_ARGS__)
#define NEW_DIV_NODE(...) NEW_FUNC_NODE(ELEM_FUNC_DIV, __VA_ARGS__)
#define NEW_POW_NODE(...) NEW_FUNC_NODE(ELEM_FUNC_POW, __VA_ARGS__)
#define NEW_LOG_NODE(...) NEW_FUNC_NODE(ELEM_FUNC_LOG, __VA_ARGS__)
#define NEW_EXP_NODE(...)  \
    NEW_POW_NODE(NEW_NUM_NODE(M_E), __VA_ARGS__)
#define NEW_LN_NODE(...)  \
    NEW_LOG_NODE(NEW_NUM_NODE(M_E), __VA_ARGS__)

#define ADD(left, right)                                                        \
    do {                                                                        \
        return NEW_ADD_NODE(DIFF(left), DIFF(right));                           \
    } while (0)

#define SUB(left, right)                                                        \
    do {                                                                        \
        return NEW_SUB_NODE(DIFF(left), DIFF(right));                           \
    } while (0)

#define MUL(left, right)                                                        \
    do {                                                                        \
        return  NEW_ADD_NODE(                                                   \
                    NEW_MUL_NODE(DIFF(left), COPY(right)),                      \
                    NEW_MUL_NODE(COPY(left), DIFF(right))                       \
                );                                                              \
    } while (0)

#define DIV(left, right)                                                        \
    do {                                                                        \
        return  NEW_DIV_NODE(                                                   \
                    NEW_SUB_NODE(                                               \
                        NEW_MUL_NODE(DIFF(left), COPY(right)),                  \
                        NEW_MUL_NODE(COPY(left), DIFF(right))                   \
                    ),                                                          \
                    NEW_FUNC_NODE(ELEM_FUNC_POW,                                \
                        COPY(right),                                            \
                        NEW_NUM_NODE(2)                                         \
                    )                                                           \
                );                                                              \
    } while (0)

//#define POW

size_t powDiffGeneralCase(size_t left, size_t right, const ArifmTree* tree, ArifmTree* destTree) {
    assert(tree     != NULL);
    assert(destTree != NULL);

    size_t lnLeftNode    = NEW_LN_NODE(left);
    size_t leftDiffNode  = DIFF(left);
    size_t rightDiffNode = DIFF(right);
    size_t mulNode       =
        NEW_MUL_NODE(
            lnLeftNode,
            COPY(right)
        );

    NEW_MUL_NODE(
        NEW_EXP_NODE(mulNode),
        DIFF(mulNode)
    );
}

size_t powDiffExponentialCase(size_t left, size_t right, const ArifmTree* tree, ArifmTree* destTree) {
    assert(tree     != NULL);
    assert(destTree != NULL);

    LOG_DEBUG_VARS(M_E, left);
    return NEW_MUL_NODE(
        DIFF(right),
        NEW_MUL_NODE(
            NEW_LN_NODE(left),
            NEW_POW_NODE(
                COPY(left),
                COPY(right)
            )
        )
    );
}

size_t powDiffBaseCase(size_t left, size_t right, const ArifmTree* tree, ArifmTree* destTree) {
    assert(tree     != NULL);
    assert(destTree != NULL);

    LOG_DEBUG_VARS(left, right, "base");
    return NEW_MUL_NODE(
        COPY(right),
        NEW_MUL_NODE(
            DIFF(left),
            NEW_POW_NODE(
                COPY(left),
                NEW_SUB_NODE(
                    COPY(right),
                    NEW_NUM_NODE(1)
                )
            )
        )
    );
}

size_t powDiffFunc(size_t left, size_t right, const ArifmTree* tree, ArifmTree* destTree) {
    assert(tree     != NULL);
    assert(destTree != NULL);

    LOG_DEBUG_VARS(left, right);
    if (left == 0 || right == 0) {
        //dumpArifmTree(tree);
        dumpArifmTree(destTree);
        openImageOfCurrentStateArifmTree(destTree);
    }
    Node* le = getArifmTreeNodePtr(destTree, left);
    Node* ri = getArifmTreeNodePtr(destTree, right);

    // both cannot be nums, because we already simplified our tree
    bool isLeftConst  = le->nodeType == ARIFM_TREE_NUMBER_NODE;
    bool isRightConst = ri->nodeType == ARIFM_TREE_NUMBER_NODE;
    assert(!isLeftConst || !isRightConst);

    if (!isLeftConst && !isRightConst) {
        return powDiffGeneralCase(left, right, tree, destTree);
    }

    if (isLeftConst) {
        return powDiffExponentialCase(left, right, tree, destTree);
    }

    return powDiffBaseCase(left, right, tree, destTree);
}

size_t logDiffNaturalLNCase(size_t left, size_t right, const ArifmTree* tree, ArifmTree* destTree) {
    assert(tree     != NULL);
    assert(destTree != NULL);

    return NEW_DIV_NODE(
        NEW_NUM_NODE(1),
        COPY(right)
    );
}

size_t logDiffGeneralCase(size_t left, size_t right, const ArifmTree* tree, ArifmTree* destTree) {
    assert(tree     != NULL);
    assert(destTree != NULL);

    return NEW_DIV_NODE(
        DIFF(right),
        COPY(right)
    );
}

size_t logDiffFunc(size_t left, size_t right, const ArifmTree* tree, ArifmTree* destTree) {
    assert(tree     != NULL);
    assert(destTree != NULL);

    // TODO:

    LOG_DEBUG_VARS(left, right);
    Node* le = getArifmTreeNodePtr(destTree, left);
    Node* ri = getArifmTreeNodePtr(destTree, right);

    // both cannot be nums, because we already simplified our tree
    bool isLeftConst  = le->nodeType == ARIFM_TREE_NUMBER_NODE;
    bool isRightConst = ri->nodeType == ARIFM_TREE_NUMBER_NODE;
    assert(!isLeftConst || !isRightConst);

    if (isLeftConst && fabsl(le->doubleData - M_E) == 0) { // natural log case
        return logDiffNaturalLNCase(left, right, tree, destTree);
    }

    return logDiffGeneralCase(left, right, tree, destTree);
    // if (!isLeftConst && !isRightConst) {
    //     return logDiffGeneralCase(left, right, tree, destTree);
    // }
}

#define POW(left, right)                                                        \
    do {                                                                        \
        return powDiffFunc(left, right, tree, destTree);                        \
    } while (0)

#define LOG(left, right)                                                        \
    do {                                                                        \
        return logDiffFunc(left, right, tree, destTree);                        \
    } while (0)

#define ROOT

#define COS(left, right)                                                        \
    do {                                                                        \
        return NEW_MUL_NODE(                                                    \
            NEW_NUM_NODE(-1),                                                   \
            NEW_MUL_NODE(                                                       \
                NEW_FUNC_NODE(ELEM_FUNC_SIN, 0, COPY(right)),                   \
                DIFF(right)                                                     \
            )                                                                   \
        );                                                                      \
    } while (0)

#define SIN(left, right)                                                        \
    do {                                                                        \
        return NEW_MUL_NODE(                                                    \
            NEW_FUNC_NODE(ELEM_FUNC_COS, 0, COPY(right)),                       \
            DIFF(right)                                                         \
        );                                                                      \
    } while (0)

#define TAN(left, right)                                                        \
    do {                                                                        \
        return NEW_DIV_NODE(                                                    \
            DIFF(right),                                                        \
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
            NEW_DIV_NODE(                                                       \
                DIFF(right),                                                    \
                NEW_FUNC_NODE(ELEM_FUNC_POW,                                    \
                    NEW_FUNC_NODE(ELEM_FUNC_SIN, 0, DIFF(right)),               \
                    NEW_NUM_NODE(2)                                             \
                )                                                               \
            )                                                                   \
        );                                                                      \
    } while (0)

#endif
