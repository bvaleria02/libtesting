#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "libtesting.h"
#include "types.h"
#include "enums.h"
#include "logger.h"
#include "singulartesthandlers.h"


double calculateAbsoluteError(double reference, double obtained){
	return fabs(reference - obtained);
}

double calculateSquaredError(double reference, double obtained){
	return powf(reference - obtained, 2);
}

double calculateRelativeError(double reference, double obtained){
	if(reference != 0){
		return fabs((reference - obtained) / reference);
	} else {
		return 0;
	}
}

ErrorCode updateMaxMinValues(SingularTest *test, double absolute, double relative, double squared){
	if(test == NULL){
		return ERROR_NULL_POINTER;
	}

	if(test->mae.minValue == 0){
		test->mae.minValue = absolute;
	}

	if(test->mre.minValue == 0){
		test->mre.minValue = relative;
	}

	if(test->mse.minValue == 0){
		test->mse.minValue = squared;
	}

	if(absolute > test->mae.maxValue){
		test->mae.maxValue = absolute;
	}

	if(relative > test->mre.maxValue){
		test->mre.maxValue = relative;
	}

	if(squared > test->mse.maxValue){
		test->mse.maxValue = squared;
	}

	if(absolute < test->mae.minValue){
		test->mae.minValue = absolute;
	}

	if(relative < test->mre.minValue){
		test->mre.minValue = relative;
	}

	if(squared < test->mse.minValue){
		test->mse.minValue = squared;
	}

	return NO_ERROR;
}

uint8_t valuesAreHigherThanThreshold(SingularTest *test, double absolute, double relative, double squared){
	if(test == NULL){
		return 0;
	}

	uint8_t result = 0;

	if(test->mae.flag && absolute > test->mae.threshold){
		result = 1;
	}

	if(test->mse.flag && squared > test->mse.threshold){
		result = 1;
	}

	if(test->rmse.flag && sqrt(squared) > test->rmse.threshold){
		result = 1;
	}

	if(test->mre.flag && relative > test->mre.threshold){
		result = 1;
	}

	return result;
}

#define UPDATE_WORST_ERRORS(test, metric, worstindex, worsterror, i) do{	\
	if(metric > (*worsterror)){												\
		(*worsterror) = metric;												\
		test->worstresults[(*worstindex)] = i;								\
		(*worstindex) = ((*worstindex) + 1) % WORST_RESULT_COUNT;			\
	}																		\
} while(0)

ErrorCode handleWorstCriteria(SingularTest *test, double absolute, double relative, double squared, uint64_t *worstindex, double *worsterror, uint64_t i){
	if(test == NULL || worstindex == NULL || worsterror == NULL){
		return ERROR_NULL_POINTER;
	}

	switch(test->worstCriteria){
		case WORST_AE:	//printf("Found worst AE\n");
						UPDATE_WORST_ERRORS(test, absolute, worstindex, worsterror, i);
						break;
		case WORST_RE:	//printf("Found worst RE\n"); 
						UPDATE_WORST_ERRORS(test, relative, worstindex, worsterror, i);
						break;
		case WORST_SE:	//printf("Found worst SE\n"); 
						UPDATE_WORST_ERRORS(test, squared, worstindex, worsterror, i);
						break;
		default:		// Do nothing
						break;
	}

	return NO_ERROR;
}


ErrorCode handleNumericTest(SingularTest *test){
	if(test == NULL){
		return ERROR_NULL_POINTER;
	}

	if(test->results == NULL || test->expectedResults == NULL){
		return ERROR_NULL_POINTER;
	}

	if(test->iterCount == 0){
		return ERROR_ITER_ZERO;
	}

	double reference = 0;
	double obtained  = 0;
	double absolute = 0;
	double squared  = 0;
	double relative = 0;
	double diff_mae = 0;
	double diff_mse = 0;
	double diff_mre = 0;

	uint64_t worstindex = 0;
	double worsterror = 0;

	ErrorCode r = NO_ERROR;

	for(uint64_t i = 0; i < test->iterCount; i++){
		reference = test->expectedResults[i];
		obtained = test->results[i];

		absolute = calculateAbsoluteError(reference, obtained);
		squared  = calculateSquaredError( reference, obtained);
		relative = calculateRelativeError(reference, obtained);

		diff_mae += absolute;
		diff_mse += squared;
		diff_mre += relative;

		r = updateMaxMinValues(test, absolute, relative, squared);
		if(r != NO_ERROR) return r;

		r = handleWorstCriteria(test, absolute, relative, squared, &worstindex, &worsterror, i);
		if(r != NO_ERROR) return r;

		if(valuesAreHigherThanThreshold(test, absolute, relative, squared)){
			continue;
		}

		test->passCount += 1;

	}

	test->mae.value  =      diff_mae / (double) test->iterCount;
	test->mse.value  =      diff_mse / (double) test->iterCount;
	test->rmse.value = sqrt(diff_mse / (double) test->iterCount);
	test->mre.value  =      diff_mre / (double) test->iterCount;

	return NO_ERROR;
}

ErrorCode handleBooleanTest(SingularTest *test){
	if(test == NULL){
		return ERROR_NULL_POINTER;
	}

	if(test->results == NULL || test->expectedResults == NULL){
		return ERROR_NULL_POINTER;
	}

	if(test->iterCount == 0){
		return ERROR_ITER_ZERO;
	}

	for(uint64_t i = 0; i < test->iterCount; i++){
		if(test->results[i] == test->expectedResults[i]){
			test->passCount += 1;
		}
	}

	return NO_ERROR;
}

ErrorCode afterTestStatus(SingularTest *test){
	if(test == NULL){
		return ERROR_NULL_POINTER;
	}

	if(test->iterCount == 0){
		return ERROR_ITER_ZERO;
	}

	double pass = test->passCount / (double)test->iterCount;
	//printf("pass: %lf\n", pass);
	if(pass >= test->passThreshold){
		test->status = STATUS_PASSED;
	} else if (pass >= (test->passThreshold - 0.2)){
		test->status = STATUS_OK;
	} else {
		test->status = STATUS_FAILED;
	}

	return NO_ERROR;
}

