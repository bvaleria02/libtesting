#ifndef LIBTESTING_TYPES_H
#define LIBTESTING_TYPES_H

#include <stdint.h>
#include <stdlib.h>

#define WORST_RESULT_COUNT 8
#define WORST_UNSET 0xFFFFFFFFFFFFFFFF

typedef struct{
	double value;
	double threshold;
	uint8_t flag;
} ErrorMetric;

typedef struct{
	char *name;				// Name of the test
	uint8_t status;			// Status: 0=untestes, 1=failed, 2=ok, 3=passed
	uint8_t type;			// Type: 0=numeric, 1=boolean
	uint64_t iterCount;		// Iter count for tests
	
	ErrorMetric mae;
	ErrorMetric mse;
	ErrorMetric rmse;
	ErrorMetric mre;

	uint8_t  worstCriteria;							// select the worst based on AE, RE, SE
	uint64_t worstresults[WORST_RESULT_COUNT];		// stores the worst N results index

	uint64_t passCount;		// Amount of passed tests
	double passThreshold;	// Threshold for pass status
	double *arguments;		// Input arguments for test function
	double *results;		// Output of test function
	double *expectedResults;// Expected output of test function
	double (*testFunction)(double *arguments, size_t argSize);
} SingularTest;

typedef struct{
	uint16_t testCount;		// Amount of Singular Tests
	uint8_t *flags;			// Flags (show histogram, export to csv)
	SingularTest **tests;	// All the tests
	char *exportPath;	    // Base name for export CSV
} TestUnit;

typedef union {
	uint64_t unsig_int;
	 int64_t signed_int;
	double 	 floatingpoint;
	void 	*pointer;
} AbstractTestType;

#define ATT_UINT(x) ((AbstractTestType) ((uint64_t) (x)))
#define ATT_INT(x)  ((AbstractTestType) (( int64_t) (x)))
#define ATT_PTR(x)  ((AbstractTestType) ((  void *) (x)))
#define ATT_FLT(x)  ((AbstractTestType) ((  double) (x)))

#endif  // LIBTESTING_TYPES_H
