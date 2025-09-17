#ifndef LIBTESTING_HISTOGRAM_H
#define LIBTESTING_HISTOGRAM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define DEFAULT_X_LENGTH 96
#define DEFAULT_Y_LENGTH 8

/*
#define DEFAULT_X_LENGTH 32
#define DEFAULT_Y_LENGTH 6
*/

void plotHistogram(double *data, size_t size, uint8_t x, uint8_t y, char *name);

#endif // LIBTESTING_HISTOGRAM_H
