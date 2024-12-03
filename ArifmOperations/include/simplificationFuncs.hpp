#ifndef ARIFM_OPERATIONS_INCLUDE_SIMPLIFICATION_FUNCS_HPP
#define ARIFM_OPERATIONS_INCLUDE_SIMPLIFICATION_FUNCS_HPP

#define NEW_NUM_NODE(data) \
    constructNodeWithKidsNoErrors(tree, NUM, {.doubleData = data}, 0, 0)

#define NEW_VAR_NODE(intData) \
    constructNodeWithKidsNoErrors(tree, VAR, {.data = intData}, 0, 0)

#define NEW_FUNC_NODE(intData, left, right) \
    constructNodeWithKidsNoErrors(tree, FUNC, {.data = intData}, left, right)

#define REDUCE_TO_CONST_AND_RETURN(number)                                                  \
    do {                                                                                    \
        *wasSimplification = true;                                                          \
        *wasVariable       = false;                                                         \
        *resultNodeInd     = NEW_NUM_NODE(number);                                          \
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
        *wasVariable       = wasVarInRight;                                                 \
        *resultNodeInd     = newRightNode;                                                  \
        return ARIFM_TREE_STATUS_OK;                                                        \
    } while (0)

#define IS_L_EQ2(num) (one == (num) && !wasVarInLeft)
#define IS_R_EQ2(num) (two == (num) && !wasVarInRight)

#define TRY_REDUCE_TO_CONST_RET_IF_OK                                       \
    do {                                                                    \
        if (!wasVarInLeft && !wasVarInRight) {                              \
            double calcRes = (*func.calculationFunc)(one, two);             \
            LOG_DEBUG_VARS(one, two, calcRes, func.name);                   \
            REDUCE_TO_CONST_AND_RETURN(calcRes);                            \
        }                                                                   \
    } while (0)

#define ADD_SIMPLIFY                                                        \
    do {                                                                    \
        if (IS_L_EQ2(0)) LEAVE_RIGHT_SON_AND_RETURN();                      \
        if (IS_R_EQ2(0)) LEAVE_LEFT_SON_AND_RETURN();                       \
    } while (0)

#define SUB_SIMPLIFY                                                        \
    do {                                                                    \
        if (IS_L_EQ2(0)) {                                                  \
            newRightNode = NEW_FUNC_NODE(ELEM_FUNC_MUL, NEW_NUM_NODE(-1), newRightNode);   \
            LEAVE_RIGHT_SON_AND_RETURN();\
        }                                                                   \
        if (IS_R_EQ2(0)) LEAVE_LEFT_SON_AND_RETURN();                       \
    } while (0)

#define MUL_SIMPLIFY                                                        \
    do {                                                                    \
        if (IS_L_EQ2(1))                LEAVE_RIGHT_SON_AND_RETURN();       \
        if (IS_R_EQ2(1))                LEAVE_LEFT_SON_AND_RETURN();        \
        if (IS_L_EQ2(0) || IS_R_EQ2(0)) REDUCE_TO_CONST_AND_RETURN(0);      \
    } while (0)

#define DIV_SIMPLIFY                                                        \
    do {                                                                    \
        if (IS_R_EQ2(1)) LEAVE_LEFT_SON_AND_RETURN();                       \
        if (IS_L_EQ2(0)) REDUCE_TO_CONST_AND_RETURN(0);                     \
        if (IS_R_EQ2(0)) {                                                  \
            /* TODO: division error */                                      \
        }                                                                   \
    } while (0)

#define POW_SIMPLIFY                                                        \
    do {                                                                    \
        if (IS_L_EQ2(1) || IS_R_EQ2(0)) REDUCE_TO_CONST_AND_RETURN(1);      \
        if (IS_R_EQ2(1))                LEAVE_LEFT_SON_AND_RETURN();        \
        if (IS_L_EQ2(0))                REDUCE_TO_CONST_AND_RETURN(0);      \
    } while (0)

#define COS_SIMPLIFY
#define SIN_SIMPLIFY
#define TAN_SIMPLIFY
#define CTAN_SIMPLIFY
#define LOG_SIMPLIFY
#define ROOT_SIMPLIFY


#endif
