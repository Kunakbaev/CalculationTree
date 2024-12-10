#define NEW_NUM_NODE(data) \
    constructNodeWithKidsNoErrors(parser->tree, NUM, {.doubleData = data}, 0, 0)

#define NEW_VAR_NODE(intData) \
    constructNodeWithKidsNoErrors(parser->tree, VAR, {.data = intData}, 0, 0)

#define NEW_FUNC_NODE(intData, left, right) \
    constructNodeWithKidsNoErrors(parser->tree, FUNC, {.data = intData}, left, right)

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

