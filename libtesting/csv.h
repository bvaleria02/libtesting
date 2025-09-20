#ifndef LIBTESTING_CSV_H
#define LIBTESTING_CSV_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "types.h"

ErrorCode exportSingularTestCSV(SingularTest *test, char *path);

#endif // LIBTESTING_CSV_H
