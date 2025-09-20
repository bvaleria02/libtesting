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


ErrorCode TimeMetricZero(TimeMetric *metric){
	if(metric == NULL){
		return ERROR_NULL_POINTER;
	}

	metric->minValue	= 0;
	metric->maxValue	= 0;
	metric->average		= 0;
	metric->elapsed		= 0;

	return NO_ERROR;
}

ErrorCode ErrorMetricZero(ErrorMetric *metric){
	if(metric == NULL){
		return ERROR_NULL_POINTER;
	}
	
	metric->value 		= 0;
	metric->flag		= 0;
	metric->threshold 	= 0;
	metric->minValue 	= 0;
	metric->maxValue 	= 0;

	return NO_ERROR;
}

ErrorCode ErrorMetricZeroTest(ErrorMetric *metric){
	if(metric == NULL){
		return ERROR_NULL_POINTER;
	}
	
	metric->value 		= 0;
	metric->minValue 	= 0;
	metric->maxValue 	= 0;

	return NO_ERROR;
}

ErrorCode SingularTestZeroAll(SingularTest *test){
	if(test == NULL){
		return ERROR_NULL_POINTER;
	}

	ErrorCode r = NO_ERROR;

	test->name 				= NULL;
	test->status			= STATUS_NOT_TESTED;
	test->type				= TYPE_UNDEFINED;
	test->iterCount 		= 0;

	r = ErrorMetricZero(&test->mae);
	if(r != NO_ERROR) return r;
	r = ErrorMetricZero(&test->mre);
	if(r != NO_ERROR) return r;
	r = ErrorMetricZero(&test->mse);
	if(r != NO_ERROR) return r;
	r = ErrorMetricZero(&test->rmse);
	if(r != NO_ERROR) return r;

	r = TimeMetricZero(&test->time);
	if(r != NO_ERROR) return r;

	test->passCount 		= 0;
	test->arguments 		= NULL;
	test->results			= NULL;
	test->expectedResults	= NULL;
	test->testFunction 		= NULL;
	test->worstCriteria		= 0;
	for(uint64_t i = 0; i < WORST_RESULT_COUNT; i++){
		test->worstresults[i] = WORST_UNSET;
	}

	return NO_ERROR;
}

ErrorCode SingularTestZeroTest(SingularTest *test){
	if(test == NULL){
		return ERROR_NULL_POINTER;
	}

	ErrorCode r = NO_ERROR;
	test->status			= STATUS_NOT_TESTED;

	r = ErrorMetricZeroTest(&test->mae);
	if(r != NO_ERROR) return r;
	r = ErrorMetricZeroTest(&test->mre);
	if(r != NO_ERROR) return r;
	r = ErrorMetricZeroTest(&test->mse);
	if(r != NO_ERROR) return r;
	r = ErrorMetricZeroTest(&test->rmse);
	if(r != NO_ERROR) return r;

	r = TimeMetricZero(&test->time);
	if(r != NO_ERROR) return r;

	test->passCount 		= 0;

	for(uint64_t i = 0; i < WORST_RESULT_COUNT; i++){
		test->worstresults[i] = WORST_UNSET;
	}

	if(test->results == NULL){
		return ERROR_NULL_POINTER;
	}

	for(uint64_t i = 0; i < test->iterCount; i++){
		test->results[i]	= 0;
	}

	return NO_ERROR;
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

	logInfo(INFO_WILL_BE_DEPRECATED, "singularTestSet", "singularTestSetPointer, singularTestSetFloat, singularTestSetInt", 0);

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


ErrorCode singularTestSetPointer(SingularTest *test, uint8_t parameter, void *value){
	char *functionName = "singularTestSetPointer";

	if(test == NULL){
		logError(ERROR_NULL_POINTER, functionName, "test", 0);
		return ERROR_NULL_POINTER;
	}


	switch(parameter){
		case ST_NAME: 			setNameHandler(test, value);
								break;
		case ST_STATUS:			logError(ERROR_WRONG_TYPE, functionName, "ST_STATUS", USE_ATT_UINT);
								return ERROR_WRONG_TYPE;
		case ST_TYPE:			logError(ERROR_WRONG_TYPE, functionName, "ST_TYPE", USE_ATT_UINT);
								return ERROR_WRONG_TYPE;
		case ST_ITERCOUNT:		logError(ERROR_WRONG_TYPE, functionName, "ST_ITERCOUNT", USE_ATT_UINT);
								return ERROR_WRONG_TYPE;
		case ST_MAETHRESHOLD:	logError(ERROR_WRONG_TYPE, functionName, "ST_MAETHRESHOLD", USE_ATT_FLOAT);
								return ERROR_WRONG_TYPE;
		case ST_MSETHRESHOLD:	logError(ERROR_WRONG_TYPE, functionName, "ST_MSETHRESHOLD", USE_ATT_FLOAT);
								return ERROR_WRONG_TYPE;
		case ST_RMSETHRESHOLD:	logError(ERROR_WRONG_TYPE, functionName, "ST_MRETHRESHOLD", USE_ATT_FLOAT);
								return ERROR_WRONG_TYPE;
		case ST_MRETHRESHOLD:	logError(ERROR_WRONG_TYPE, functionName, "ST_RMSETHRESHOLD", USE_ATT_FLOAT);
								return ERROR_WRONG_TYPE;
		case ST_PASSTHRESHOLD:	logError(ERROR_WRONG_TYPE, functionName, "ST_PASSTHRESHOLD", USE_ATT_FLOAT);
								return ERROR_WRONG_TYPE;
		case ST_TESTFUNCTION:	if(value == NULL){
									logError(ERROR_NULL_POINTER, functionName, "value", 0);
									return ERROR_NULL_POINTER;
								}
								test->testFunction = value;
								break;
		case ST_THRESHOLDFLAG:	logError(ERROR_WRONG_TYPE, functionName, "ST_THRESHOLDFLAG", USE_ATT_UINT);
								return ERROR_WRONG_TYPE;
		case ST_WORSTCRITERIA:	logError(ERROR_WRONG_TYPE, functionName, "ST_WORSTCRITERIA", USE_ATT_UINT);
								return ERROR_WRONG_TYPE;
	}

	return NO_ERROR;
}

ErrorCode singularTestSetFloat(SingularTest *test, uint8_t parameter, double value){
	char *functionName = "singularTestSetFloat";

	if(test == NULL){
		logError(ERROR_NULL_POINTER, functionName, "test", 0);
		return ERROR_NULL_POINTER;
	}

	switch(parameter){
		case ST_NAME: 			logError(ERROR_WRONG_TYPE, functionName, "ST_NAME", USE_ATT_POINTER);
								return ERROR_WRONG_TYPE;
		case ST_STATUS:			logError(ERROR_WRONG_TYPE, functionName, "ST_STATUS", USE_ATT_UINT);
								return ERROR_WRONG_TYPE;
		case ST_TYPE:			logError(ERROR_WRONG_TYPE, functionName, "ST_TYPE", USE_ATT_UINT);
								return ERROR_WRONG_TYPE;
		case ST_ITERCOUNT:		logError(ERROR_WRONG_TYPE, functionName, "ST_ITERCOUNT", USE_ATT_UINT);
								return ERROR_WRONG_TYPE;
		case ST_MAETHRESHOLD:	test->mae.threshold = value;
								break;
		case ST_MSETHRESHOLD:	test->mse.threshold = value;
								break;
		case ST_RMSETHRESHOLD:	test->rmse.threshold = value;
								break;
		case ST_MRETHRESHOLD:	test->mre.threshold = value;
								break;
		case ST_PASSTHRESHOLD:	test->passThreshold = value;
								break;
		case ST_TESTFUNCTION:	logError(ERROR_WRONG_TYPE, functionName, "ST_TESTFUNCTION", USE_ATT_TESTFUNCTION);
								return ERROR_WRONG_TYPE;
		case ST_THRESHOLDFLAG:	logError(ERROR_WRONG_TYPE, functionName, "ST_THRESHOLDFLAG", USE_ATT_UINT);
								return ERROR_WRONG_TYPE;
		case ST_WORSTCRITERIA:	logError(ERROR_WRONG_TYPE, functionName, "ST_WORSTCRITERIA", USE_ATT_UINT);
								return ERROR_WRONG_TYPE;
	}

	return NO_ERROR;
}

ErrorCode singularTestSetInt(SingularTest *test, uint8_t parameter, int64_t value){
	char *functionName = "singularTestSetInt";

	if(test == NULL){
		logError(ERROR_NULL_POINTER, functionName, "test", 0);
		return ERROR_NULL_POINTER;
	}

	switch(parameter){
		case ST_NAME: 			logError(ERROR_WRONG_TYPE, functionName, "ST_NAME", USE_ATT_POINTER);
								return ERROR_WRONG_TYPE;
		case ST_STATUS:			test->status = value;
								break;
		case ST_TYPE:			test->type = value;
								break;
		case ST_ITERCOUNT:		setIterCountHandler(test, value);
								break;
		case ST_MAETHRESHOLD:	logError(ERROR_WRONG_TYPE, functionName, "ST_MAETHRESHOLD", USE_ATT_FLOAT);
								return ERROR_WRONG_TYPE;
		case ST_MSETHRESHOLD:	logError(ERROR_WRONG_TYPE, functionName, "ST_MSETHRESHOLD", USE_ATT_FLOAT);
								return ERROR_WRONG_TYPE;
		case ST_RMSETHRESHOLD:	logError(ERROR_WRONG_TYPE, functionName, "ST_MRETHRESHOLD", USE_ATT_FLOAT);
								return ERROR_WRONG_TYPE;
		case ST_MRETHRESHOLD:	logError(ERROR_WRONG_TYPE, functionName, "ST_RMSETHRESHOLD", USE_ATT_FLOAT);
								return ERROR_WRONG_TYPE;
		case ST_PASSTHRESHOLD:	logError(ERROR_WRONG_TYPE, functionName, "ST_PASSTHRESHOLD", USE_ATT_FLOAT);
								return ERROR_WRONG_TYPE;
		case ST_TESTFUNCTION:	logError(ERROR_WRONG_TYPE, functionName, "ST_TESTFUNCTION", USE_ATT_TESTFUNCTION);
								return ERROR_WRONG_TYPE;
		case ST_THRESHOLDFLAG:	setMetricFlagHandler(test, value & 0xF);
								break;
		case ST_WORSTCRITERIA:	test->worstCriteria = value & 0x3;
								break;
	}

	return NO_ERROR;
}

ErrorCode destroySingularTest(SingularTest *test){
	if(test == NULL){
		return ERROR_NULL_POINTER;
	}
	
	SAFE_FREE(test->name);
	SAFE_FREE(test->arguments);
	SAFE_FREE(test->results);
	SAFE_FREE(test->expectedResults);
	SAFE_FREE(test);

	return NO_ERROR;
}

ErrorCode handeTimerSingularTest(SingularTest *test, double time){
	if(test == NULL){
		return ERROR_NULL_POINTER;
	}
	
	if(time > test->time.maxValue){
		test->time.maxValue = time;
	}

	if(time < test->time.minValue || test->time.minValue == 0){
		test->time.minValue = time;
	}

	test->time.elapsed += time;
	return NO_ERROR;
}

ErrorCode handlePostTestTimerSingularTest(SingularTest *test){
	if(test == NULL){
		return ERROR_NULL_POINTER;
	}

	test->time.average = test->time.elapsed / (double) test->iterCount;
	return NO_ERROR;
}

ErrorCode SingularTestRun(SingularTest *test){
	if(test == NULL){
		logError(ERROR_NULL_POINTER, "SingularTestRun", "test", 0);
		return ERROR_NULL_POINTER;
	}

	if(test->testFunction == NULL){
		logError(ERROR_NULL_POINTER, "SingularTestRun", "test->testFunction", 0);
		return ERROR_NULL_POINTER;
	}
	
	if(test->iterCount == 0){
		logError(ERROR_ITER_ZERO, "SingularTestRun", "test->iterCount", 0);
		return ERROR_ITER_ZERO;
	}

	if(test->arguments == NULL){
		logError(ERROR_NULL_POINTER, "SingularTestRun", "test->arguments", 0);
		return ERROR_NULL_POINTER;
	}

	if(test->results == NULL){
		logError(ERROR_NULL_POINTER, "SingularTestRun", "test->results", 0);
		return ERROR_NULL_POINTER;
	}

	if(test->type == TYPE_UNDEFINED){
		logError(ERROR_TYPE_UNDEF, "SingularTestRun", "test->type", 0);
		return ERROR_TYPE_UNDEF;
	}

	ErrorCode r = NO_ERROR;

	r = SingularTestZeroTest(test);
	if(r != NO_ERROR) return r;

	double time = 0;
	double result = 0;
	for(uint64_t i = 0; i < test->iterCount; i++){
		result = test->testFunction(test->arguments, i, &time);
		test->results[i] = result;
		r = handeTimerSingularTest(test, time);
		if(r != NO_ERROR) return r;
	}

	r = handlePostTestTimerSingularTest(test);
	if(r != NO_ERROR) return r;

	test->status = STATUS_TESTED;
	
	switch(test->type){
		case TYPE_NUMERIC:	r = handleNumericTest(test);
							if(r != NO_ERROR) return r;
							break;
		case TYPE_BOOLEAN:	r = handleBooleanTest(test);
							if(r != NO_ERROR) return r;
							break;
		default:			
							break;
	}

	r = afterTestStatus(test);
	if(r != NO_ERROR) return r;

	return NO_ERROR;
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
