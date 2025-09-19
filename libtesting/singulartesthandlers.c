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

void handleNumericTest(SingularTest *test){
	if(test == NULL){
		return;
	}

	if(test->results == NULL || test->expectedResults == NULL){
		return;
	}

	if(test->iterCount == 0){
		return;
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

	for(uint64_t i = 0; i < test->iterCount; i++){
		reference = test->expectedResults[i];
		obtained = test->results[i];

		absolute = calculateAbsoluteError(reference, obtained);
		squared  = calculateSquaredError( reference, obtained);
		relative = calculateRelativeError(reference, obtained);

		diff_mae += absolute;
		diff_mse += squared;
		diff_mre += relative;

		switch(test->worstCriteria){
			case WORST_AE:	//printf("Found worst AE\n");
							if(absolute > worsterror){
								worsterror = absolute;
								test->worstresults[worstindex] = i;
								worstindex = (worstindex + 1) % WORST_RESULT_COUNT;
							}
							break;
			case WORST_SE:	//printf("Found worst SE\n"); 
							if(squared > worsterror){
								worsterror = squared;
								test->worstresults[worstindex] = i;
								worstindex = (worstindex + 1) % WORST_RESULT_COUNT;
							}
							break;
			case WORST_RE:	//printf("Found worst RE\n"); 
							if(relative > worsterror){
								worsterror = relative;
								test->worstresults[worstindex] = i;
								worstindex = (worstindex + 1) % WORST_RESULT_COUNT;
							}
							break;
			default:		// Do nothing
							break;
		}

		if(test->mae.flag && absolute > test->mae.threshold){
			continue;
		}

		if(test->mse.flag && squared > test->mse.threshold){
			continue;
		}

		if(test->rmse.flag && sqrt(squared) > test->rmse.threshold){
			continue;
		}

		if(test->mre.flag && relative > test->mre.threshold){
			continue;
		}

		test->passCount += 1;

	}

	test->mae.value  =      diff_mae / (double) test->iterCount;
	test->mse.value  =      diff_mse / (double) test->iterCount;
	test->rmse.value = sqrt(diff_mse / (double) test->iterCount);
	test->mre.value  =      diff_mre / (double) test->iterCount;
}

void handleBooleanTest(SingularTest *test){
	if(test == NULL){
		return;
	}

	if(test->results == NULL || test->expectedResults == NULL){
		return;
	}

	if(test->iterCount == 0){
		return;
	}

	for(uint64_t i = 0; i < test->iterCount; i++){
		if(test->results[i] == test->expectedResults[i]){
			test->passCount += 1;
		}
	}
}

void afterTestStatus(SingularTest *test){
	if(test == NULL){
		return;
	}

	if(test->iterCount == 0){
		return;
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
}

