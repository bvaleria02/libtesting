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
#include "utils.h"


void TimeMetricZero(TimeMetric *metric){
	if(metric == NULL){
		return;
	}

	metric->minValue	= 0;
	metric->maxValue	= 0;
	metric->average		= 0;
	metric->elapsed		= 0;
}

void ErrorMetricZero(ErrorMetric *metric){
	if(metric == NULL){
		return;
	}
	
	metric->value 		= 0;
	metric->flag		= 0;
	metric->threshold 	= 0;
	metric->minValue 	= 0;
	metric->maxValue 	= 0;
}

void ErrorMetricZeroTest(ErrorMetric *metric){
	if(metric == NULL){
		return;
	}
	
	metric->value 		= 0;
	metric->minValue 	= 0;
	metric->maxValue 	= 0;
}

void SingularTestZeroAll(SingularTest *test){
	if(test == NULL){
		return;
	}

	test->name 				= NULL;
	test->status			= STATUS_NOT_TESTED;
	test->type				= TYPE_UNDEFINED;
	test->iterCount 		= 0;

	ErrorMetricZero(&test->mae);
	ErrorMetricZero(&test->mre);
	ErrorMetricZero(&test->mse);
	ErrorMetricZero(&test->rmse);

	TimeMetricZero(&test->time);

	test->passCount 		= 0;
	test->arguments 		= NULL;
	test->results			= NULL;
	test->expectedResults	= NULL;
	test->testFunction 		= NULL;
	test->worstCriteria		= 0;
	for(uint64_t i = 0; i < WORST_RESULT_COUNT; i++){
		test->worstresults[i] = WORST_UNSET;
	}
}

void SingularTestZeroTest(SingularTest *test){
	if(test == NULL){
		return;
	}

	test->status			= STATUS_NOT_TESTED;

	ErrorMetricZeroTest(&test->mae);
	ErrorMetricZeroTest(&test->mre);
	ErrorMetricZeroTest(&test->mse);
	ErrorMetricZeroTest(&test->rmse);

	TimeMetricZero(&test->time);

	test->passCount 		= 0;

	for(uint64_t i = 0; i < WORST_RESULT_COUNT; i++){
		test->worstresults[i] = WORST_UNSET;
	}

	if(test->results == NULL){
		return;
	}

	for(uint64_t i = 0; i < test->iterCount; i++){
		test->results[i]	= 0;
	}
}

SingularTest *createNewSingularTest(){
	SingularTest *test = (SingularTest *)malloc(sizeof(SingularTest));
	if(test == NULL){
		return NULL;
	}

	SingularTestZeroAll(test);
	return test;
}

void setNameHandler(SingularTest *test, char *name){
	// Return if test or name are NULL
	if(name == NULL || test == NULL){
		return;
	}

	// Free the last name (if there is one)
	if(test->name != NULL){
		free(test->name);
		test->name = NULL;
	}

	// Allocate memory for new name. Returns if fail
	size_t namelen = strlen(name);
	test->name = (char *)malloc(namelen * sizeof(char) + 1);
	if(test->name == NULL){
		return;
	}

	// Copy string and ensures NULL terminator
	strncpy(test->name, name, namelen);
	test->name[namelen] = '\0';
}

void setIterCountHandler(SingularTest *test, uint64_t value){
	// Check if test is NULL
	if(test == NULL){
		return;
	}

	if(value == 0){
		logError(ERROR_ZERO_LENGTH, "setIterCountHandler", NULL, 0);
		return;
	}

	// Calculate memory size
	size_t chunkSize = sizeof(double) * value;

	// Free the arguments chunk 
	SAFE_FREE(test->arguments);

	// Allocate memory for arguments chunk
	test->arguments = (double *)malloc(chunkSize);
	if(test->arguments == NULL){
		logError(ERROR_MALLOC, "setIterCountHandler", "test->arguments", chunkSize);
		goto cleanup;
	}
	memset(test->arguments, 0, chunkSize);

	// Free the results chunk 
	SAFE_FREE(test->results);

	// Allocate memory for results chunk
	test->results = (double *)malloc(chunkSize);
	if(test->results == NULL){
		logError(ERROR_MALLOC, "setIterCountHandler", "test->results", chunkSize);
		goto cleanup;
	}
	memset(test->results, 0, chunkSize);
	
	// Free the results chunk 
	SAFE_FREE(test->expectedResults);

	// Allocate memory for expectedResults chunk
	test->expectedResults = (double *)malloc(chunkSize);
	if(test->expectedResults == NULL){
		logError(ERROR_MALLOC, "setIterCountHandler", "test->expectedResults", chunkSize);
		goto cleanup;
	}
	memset(test->expectedResults, 0, chunkSize);
	
	// Set iterCount to value
	test->iterCount = value;

	return;

cleanup:
	SAFE_FREE(test->arguments);
	SAFE_FREE(test->results);
	SAFE_FREE(test->expectedResults);
	test->iterCount = 0;
}

void setMetricFlagHandler(SingularTest *test, uint8_t flags){
	if(test == NULL){
		return;
	}

	test->mae.flag  = setFlag(test->mae.flag,  THRES_MAE,  flags);
	test->mre.flag  = setFlag(test->mre.flag,  THRES_MRE,  flags);
	test->mse.flag  = setFlag(test->mse.flag,  THRES_MSE,  flags);
	test->rmse.flag = setFlag(test->rmse.flag, THRES_RMSE, flags);
}

void singularTestSet(SingularTest *test, uint8_t parameter, AbstractTestType value){
	if(test == NULL){
		return;
	}

	switch(parameter){
		case ST_NAME: 			setNameHandler(test, value.pointer);
								break;
		case ST_STATUS:			test->status = value.unsig_int;
								break;
		case ST_TYPE:			test->type = value.unsig_int;
								break;
		case ST_ITERCOUNT:		setIterCountHandler(test, value.unsig_int);
								break;
		case ST_MAETHRESHOLD:	test->mae.threshold = value.floatingpoint;
								break;
		case ST_MSETHRESHOLD:	test->mse.threshold = value.floatingpoint;
								break;
		case ST_RMSETHRESHOLD:	test->rmse.threshold = value.floatingpoint;
								break;
		case ST_MRETHRESHOLD:	test->mre.threshold = value.floatingpoint;
								break;
		case ST_PASSTHRESHOLD:	test->passThreshold = value.floatingpoint;
								break;
		case ST_TESTFUNCTION:	if(value.pointer == NULL) return;
								test->testFunction = value.pointer;
								break;
		case ST_THRESHOLDFLAG:	setMetricFlagHandler(test, value.unsig_int & 0xF);
								break;
		case ST_WORSTCRITERIA:	test->worstCriteria = value.unsig_int & 0x3;
								break;
	}
}

void destroySingularTest(SingularTest *test){
	if(test == NULL){
		return;
	}
	
	SAFE_FREE(test->name);
	SAFE_FREE(test->arguments);
	SAFE_FREE(test->results);
	SAFE_FREE(test->expectedResults);
	SAFE_FREE(test);
}

void handeTimerSingularTest(SingularTest *test, double time){
	if(test == NULL){
		return;
	}
	
	if(time > test->time.maxValue){
		test->time.maxValue = time;
	}

	if(time < test->time.minValue || test->time.minValue == 0){
		test->time.minValue = time;
	}

	test->time.elapsed += time;
}

void handlePostTestTimerSingularTest(SingularTest *test){
	if(test == NULL){
		return;
	}

	test->time.average = test->time.elapsed / (double) test->iterCount;
}

void SingularTestRun(SingularTest *test){
	if(test == NULL){
		logError(ERROR_NULL_POINTER, "SingularTestRun", "test", 0);
		return;
	}

	if(test->testFunction == NULL){
		logError(ERROR_NULL_POINTER, "SingularTestRun", "test->testFunction", 0);
		return;
	}
	
	if(test->iterCount == 0){
		logError(ERROR_ITER_ZERO, "SingularTestRun", "test->iterCount", 0);
		return;
	}

	if(test->arguments == NULL){
		logError(ERROR_NULL_POINTER, "SingularTestRun", "test->arguments", 0);
		return;
	}

	if(test->results == NULL){
		logError(ERROR_NULL_POINTER, "SingularTestRun", "test->results", 0);
		return;
	}

	if(test->type == TYPE_UNDEFINED){
		logError(ERROR_TYPE_UNDEF, "SingularTestRun", "test->type", 0);
		return;
	}

	SingularTestZeroTest(test);

	double time = 0;
	double result = 0;
	for(uint64_t i = 0; i < test->iterCount; i++){
		result = test->testFunction(test->arguments, i, &time);
		test->results[i] = result;
		handeTimerSingularTest(test, time);
	}

	handlePostTestTimerSingularTest(test);

	test->status = STATUS_TESTED;
	
	switch(test->type){
		case TYPE_NUMERIC:	handleNumericTest(test);
							break;
		case TYPE_BOOLEAN:	handleBooleanTest(test);
							break;
		default:			
							break;
	}

	afterTestStatus(test);
}

void copyErroMetric(ErrorMetric *dest, ErrorMetric *src){
	if(dest == NULL || src == NULL){
		logError(ERROR_NULL_POINTER, "copyErroMetric", "dest or src", 0);
		return;
	}

	dest->flag		= src->flag;
	dest->value 	= src->value;
	dest->threshold = src->threshold;
}

void copyDetailsSingularTest(SingularTest *dest, SingularTest *src){
	if(dest == NULL || src == NULL){
		logError(ERROR_NULL_POINTER, "copySingularTest", "dest or src", 0);
		return;
	}

	dest->name			= NULL;
	dest->status 		= STATUS_NOT_TESTED;
	dest->type   		= src->type;

	copyErroMetric(&dest->mae, &src->mae);
	copyErroMetric(&dest->mre, &src->mre);
	copyErroMetric(&dest->mse, &src->mse);
	copyErroMetric(&dest->rmse, &src->rmse);

	dest->passThreshold = src->passThreshold;
	dest->testFunction  = NULL;
	dest->worstCriteria = src->worstCriteria;
	singularTestSet(dest, ST_ITERCOUNT, ATT_UINT(src->iterCount));
}

uint8_t getSingularTestErrorFlag(SingularTest *test, uint8_t flag){
	if(test == NULL){
		return 0;
	}

	uint8_t result = 0;

	if(getFlag(flag, THRES_MAE)){
		result = result | test->mae.flag;
	}
	if(getFlag(flag, THRES_MRE)){
		result = result | test->mre.flag;
	}
	if(getFlag(flag, THRES_MSE)){
		result = result | test->mse.flag;
	}
	if(getFlag(flag, THRES_RMSE)){
		result = result | test->rmse.flag;
	}

	return result;
}
