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

void SingularTestZeroAll(SingularTest *test){
	if(test == NULL){
		return;
	}

	test->name 				= NULL;
	test->status			= STATUS_NOT_TESTED;
	test->type				= TYPE_UNDEFINED;
	test->iterCount 		= 0;
	test->mae				= 0;
	test->mse				= 0;
	test->rmse				= 0;
	test->mre				= 0;
	test->thresholdflag		= 0;
	test->maethreshold 		= 0;
	test->msethreshold 		= 0;
	test->rmsethreshold		= 0;
	test->mrethreshold 		= 0;
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
	test->mae				= 0;
	test->mse				= 0;
	test->rmse				= 0;
	test->mre				= 0;
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
		case ST_MAETHRESHOLD:	test->maethreshold = value.floatingpoint;
								break;
		case ST_MSETHRESHOLD:	test->msethreshold = value.floatingpoint;
								break;
		case ST_RMSETHRESHOLD:	test->rmsethreshold = value.floatingpoint;
								break;
		case ST_MRETHRESHOLD:	test->mrethreshold = value.floatingpoint;
								break;
		case ST_PASSTHRESHOLD:	test->passThreshold = value.floatingpoint;
								break;
		case ST_TESTFUNCTION:	if(value.pointer == NULL) return;
								test->testFunction = value.pointer;
								break;
		case ST_THRESHOLDFLAG:	test->thresholdflag = value.unsig_int & 0xF;
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

	double result = 0;
	for(uint64_t i = 0; i < test->iterCount; i++){
//		printf("i: %li\n", i);
		result = test->testFunction(test->arguments, i);
//		printf("r: %lf\n", result);
		test->results[i] = result;
	}

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

void copyDetailsSingularTest(SingularTest *dest, SingularTest *src){
	if(dest == NULL || src == NULL){
		logError(ERROR_NULL_POINTER, "copySingularTest", "dest or src", 0);
		return;
	}

	dest->name			= NULL;
	dest->status 		= STATUS_NOT_TESTED;
	dest->type   		= src->type;
	dest->maethreshold 	= src->maethreshold;
	dest->msethreshold 	= src->msethreshold;
	dest->rmsethreshold = src->rmsethreshold;
	dest->mrethreshold 	= src->mrethreshold;
	dest->thresholdflag = src->thresholdflag;
	dest->passThreshold = src->passThreshold;
	dest->testFunction  = NULL;
	dest->worstCriteria = src->worstCriteria;
	singularTestSet(dest, ST_ITERCOUNT, ATT_UINT(src->iterCount));
}
