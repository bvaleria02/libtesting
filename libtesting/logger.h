#ifndef LIBTESTING_LOGGER_H
#define LIBTESTING_LOGGER_H

#include <stdio.h>
#include <stdint.h>
#include "types.h"

void logError(uint8_t errorCode, char *functionName, char *details, uint64_t value);
void logInfo(uint8_t infoCode, char *functionName, char *details, uint64_t value);

ErrorCode logSingularTest(SingularTest *test);
ErrorCode logSingularTestHistogram(SingularTest *test, uint8_t type);

#endif // LIBTESTING_LOGGER_H
