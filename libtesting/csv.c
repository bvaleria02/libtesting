#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "types.h"
#include "enums.h"
#include "logger.h"
#include "csv.h"

ErrorCode exportSingularTestCSV(SingularTest *test, char *path){
	if(test == NULL){
		logError(ERROR_NULL_POINTER, "exportSingularTestCSV", "test", 0);
		return ERROR_NULL_POINTER;
	}

	if(path == NULL){
		logError(ERROR_NULL_POINTER, "exportSingularTestCSV", "path", 0);
		return ERROR_NULL_POINTER;
	}

	if(test->arguments == NULL){
		logError(ERROR_NULL_POINTER, "exportSingularTestCSV", "test->arguments", 0);
		return ERROR_NULL_POINTER;
	}

	if(test->results == NULL){
		logError(ERROR_NULL_POINTER, "exportSingularTestCSV", "test->results", 0);
		return ERROR_NULL_POINTER;
	}

	if(test->expectedResults == NULL){
		logError(ERROR_NULL_POINTER, "exportSingularTestCSV", "test->expectedResults", 0);
		return ERROR_NULL_POINTER;
	}

	if(test->status == STATUS_NOT_TESTED){
		logError(ERROR_NOTTESTED, "exportSingularTestCSV", NULL, 0);
		return ERROR_NOTTESTED;
	}

	FILE *fp = fopen(path, "w+");
	if(fp == NULL){
		logError(ERROR_FILE, "exportSingularTestCSV", path, 0);
		return ERROR_FILE;
	}

	fprintf(fp, "n, arguments, expectedResults, results\n");

	for(uint64_t i = 0; i < test->iterCount; i++){
		fprintf(fp, "%li,%lf,%lf,%lf\n", i, test->arguments[i], test->expectedResults[i], test->results[i]);
	}

	fclose(fp);
	logInfo(INFO_FILE_SUCCESS, "exportSingularTestCSV", path, 0);

	return NO_ERROR;
}
