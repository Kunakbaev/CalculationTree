#ifndef ARIFM_OPERATIONS_INCLUDE_FUNCTIONS_DIFF_DEFINES_HPP
#define ARIFM_OPERATIONS_INCLUDE_FUNCTIONS_DIFF_DEFINES_HPP

#define COPY(srcNodeInd) getCopyOfSubtree(tree, destTree, srcNodeInd, 0, false)

#define NEW_NUM_NODE(data) \
    constructNodeWithKidsNoErrors(destTree, NUM, {.doubleData = data}, 0, 0)

#define NEW_VAR_NODE(intData) \
    constructNodeWithKidsNoErrors(destTree, VAR, {.data = intData}, 0, 0)

#define NEW_FUNC_NODE(intData, left, right) \
    constructNodeWithKidsNoErrors(destTree, FUNC, {.data = intData}, left, right)

#define DIFF(node) \
    findDerivativeOfNode(tree, node, destTree)

#define FUNC ARIFM_TREE_FUNC_NODE
#define NUM  ARIFM_TREE_NUMBER_NODE
#define VAR  ARIFM_TREE_VAR_NODE

#define NEW_ADD_NODE(...) NEW_FUNC_NODE(ELEM_FUNC_ADD, __VA_ARGS__)
#define NEW_SUB_NODE(...) NEW_FUNC_NODE(ELEM_FUNC_SUB, __VA_ARGS__)
#define NEW_MUL_NODE(...) NEW_FUNC_NODE(ELEM_FUNC_MUL, __VA_ARGS__)
#define NEW_DIV_NODE(...) NEW_FUNC_NODE(ELEM_FUNC_DIV, __VA_ARGS__)

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

#define POW
#define LOG
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
