#include <ctype.h>
#include <math.h>

#include "../include/commonFileStart.hpp"
#include "../include/assingParents.hpp"

constexpr const size_t NUM_OF_POINTS = 100;
constexpr const double EPS   = 1e-6;
static_assert(NUM_OF_POINTS >= 5);

static void reduceYpoint(double* y, double minY, double maxY) {
    assert(y != NULL);
    if (*y - minY < -EPS)
        *y = minY;
    if (*y - maxY > EPS)
        *y = maxY;
}

static ArifmTreeErrors savePointsToFile(const ArifmTree* tree,
                                        double minX, double maxX,
                                        double minY, double maxY,
                                        const char* color, const char* label) {
    IF_ARG_NULL_RETURN(tree);
    IF_NOT_COND_RETURN(minX < maxX, ARIFM_TREE_INVALID_ARGUMENT);
    IF_NOT_COND_RETURN(minY < maxY, ARIFM_TREE_INVALID_ARGUMENT);

    FILE* file = fopen("pythonScripts/plotPoints.txt", "a");
    IF_NOT_COND_RETURN(file != NULL, ARIFM_TREE_FILE_OPENING_ERROR);
    double dxStep = (maxX - minX) / NUM_OF_POINTS;
    fprintf(file, "\n");
    fprintf(file, "%s %s\n", color, label);
    for (double point = minX; maxX - point > EPS; point += dxStep) {
        double y = 0;
        IF_ERR_RETURN(substitutePointToTree(tree, tree->root, point, &y));
        LOG_DEBUG_VARS(point, y);
        reduceYpoint(&y, minY, maxY);
        fprintf(file, "%f %f\n", point, y);
    }
    fclose(file);

    return ARIFM_TREE_STATUS_OK;
}

void drawAllPlots() {
    system("python3 pythonScripts/showPlotByDots.py");
}

ArifmTreeErrors addArifmTreePlot(const ArifmTree* tree,
                                   double minX, double maxX,
                                   double minY, double maxY,
                                   const char* color, const char* label) {
    IF_ARG_NULL_RETURN(tree);

    IF_ERR_RETURN(savePointsToFile(tree, minX, maxX, minY, maxY, color, label));

    return ARIFM_TREE_STATUS_OK;
}
