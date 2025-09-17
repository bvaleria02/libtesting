#ifndef LIBTESTING_SINGULARTEST_HANDLERS_H
#define LIBTESTING_SINGULARTEST_HANDLERS_H

#include "types.h"

double calculateAbsoluteError(double reference, double obtained);
double calculateSquaredError(double reference, double obtained);
double calculateRelativeError(double reference, double obtained);

void handleNumericTest(SingularTest *test);
void handleBooleanTest(SingularTest *test);
void afterTestStatus(SingularTest *test);

#endif // LIBTESTING_SINGULARTEST_HANDLERS_H
