#ifndef ARIFM_OPERATIONS_INCLUDE_FUNCTIONS_REALIZATIONS_HPP
#define ARIFM_OPERATIONS_INCLUDE_FUNCTIONS_REALIZATIONS_HPP

#include <cstddef>
#include <cassert>

typedef double (*functionsRealizationPtr)(double a, double b);

double add2nums(double a, double b);
double sub2nums(double a, double b);
double mul2nums(double a, double b);
double div2nums(double a, double b);

#endif
