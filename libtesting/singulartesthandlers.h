#ifndef LIBTESTING_SINGULARTEST_HANDLERS_H
#define LIBTESTING_SINGULARTEST_HANDLERS_H

#include "types.h"

double calculateAbsoluteError(double reference, double obtained);
double calculateSquaredError(double reference, double obtained);
double calculateRelativeError(double reference, double obtained);

ErrorCode handleNumericTest(SingularTest *test);
ErrorCode handleBooleanTest(SingularTest *test);
ErrorCode afterTestStatus(SingularTest *test);

#endif // LIBTESTING_SINGULARTEST_HANDLERS_H
