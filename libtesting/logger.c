#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "libtesting.h"
#include "logger.h"
#include "enums.h"
#include "termColor.h"
#include "singulartesthandlers.h"
#include "histogram.h"

FILE *globalLogTarget = NULL;

void printFunctionName(FILE *logTarget, char *functionName){
	if(logTarget == NULL){
		logTarget = stdout;
	}

	if(functionName == NULL){
		fprintf(logTarget, NORMAL_RED "(NULL)" RESET_STYLE);
		return;
	}

	fprintf(logTarget, "<" NORMAL_YELLOW "%s" RESET_STYLE ">", functionName);
}

void printDetails(FILE *logTarget, char *details){
	if(logTarget == NULL){
		logTarget = stdout;
	}

	if(details == NULL){
		fprintf(logTarget, NORMAL_RED "(NULL)" RESET_STYLE);
		return;
	}

	fprintf(logTarget, NORMAL_CYAN "%s" RESET_STYLE, details);
}

void logError(uint8_t errorCode, char *functionName, char *details, uint64_t value){
	FILE *logTarget = globalLogTarget;

	if(logTarget == NULL){
		logTarget = stderr;
	}

	fprintf(logTarget, RESET_STYLE "[" BOLD_RED "ERROR" RESET_STYLE "] ");

	switch(errorCode){
		case ERROR_MALLOC: 		fprintf(logTarget, "Malloc error allocating " NORMAL_GREEN "%li" RESET_STYLE " bytes in ", value);
								printFunctionName(logTarget, functionName);
								fprintf(logTarget, " at ");
								printDetails(logTarget, details);
								fprintf(logTarget, "\n");
								break;
		case ERROR_ZERO_LENGTH: 
								fprintf(logTarget, "Length error, can't pass length 0 to ");
								printFunctionName(logTarget, functionName);
								fprintf(logTarget, "\n");
								break;
		case ERROR_NULL_POINTER:
								fprintf(logTarget, "NULL pointer passed in ");
								printFunctionName(logTarget, functionName);
								fprintf(logTarget, " at ");
								printDetails(logTarget, details);
								fprintf(logTarget, "\n");
								break;
		case ERROR_ITER_ZERO: 
								fprintf(logTarget, "Set 0 to iteration count in ");
								printFunctionName(logTarget, functionName);
								fprintf(logTarget, " at ");
								printDetails(logTarget, details);
								fprintf(logTarget, "\n");
								break;
		case ERROR_TYPE_UNDEF: 
								fprintf(logTarget, "Undefined type for test in ");
								printFunctionName(logTarget, functionName);
								fprintf(logTarget, " at ");
								printDetails(logTarget, details);
								fprintf(logTarget, "\n");
								break;
		case ERROR_INDEX_TOOBIG: 
								fprintf(logTarget, "Index out of bound in ");
								printFunctionName(logTarget, functionName);
								fprintf(logTarget, " at ");
								printDetails(logTarget, details);
								fprintf(logTarget, "\n");
								break;
		case ERROR_FILE: 
								fprintf(logTarget, "Can't open file in ");
								printFunctionName(logTarget, functionName);
								fprintf(logTarget, " with name ");
								printDetails(logTarget, details);
								fprintf(logTarget, "\n");
								break;
		case ERROR_NOTTESTED: 
								fprintf(logTarget, "Passed test has not run. In");
								printFunctionName(logTarget, functionName);
								fprintf(logTarget, "\n");
								break;
		default:
								fprintf(logTarget, "Unespecified error; [");
								printFunctionName(logTarget, functionName);
								fprintf(logTarget, ", ");
								printDetails(logTarget, details);
								fprintf(logTarget, ", %li]\n", value);
	}
}

void logInfo(uint8_t infoCode, char *functionName, char *details, uint64_t value){
	FILE *logTarget = globalLogTarget;

	if(logTarget == NULL){
		logTarget = stdout;
	}

	fprintf(logTarget, RESET_STYLE "[" BOLD_MAGENTA "INFO" RESET_STYLE "] ");

	switch(infoCode){
		case INFO_FILE_SUCCESS:	
								fprintf(logTarget, "File successfuly written! In ");
								printFunctionName(logTarget, functionName);
								fprintf(logTarget, " to ");
								printDetails(logTarget, details);
								fprintf(logTarget, "\n");
								break;
		case INFO_SKIPPED_TEST:	
								fprintf(logTarget, "Skipping test " NORMAL_GREEN "%li" RESET_STYLE " in ", value);
								printFunctionName(logTarget, functionName);
								fprintf(logTarget, ", test name: ");
								printDetails(logTarget, details);
								fprintf(logTarget, "\n");
								break;
		case INFO_EXPORT_PATH_UNSET:	
								fprintf(logTarget, "Export path unset in ");
								printFunctionName(logTarget, functionName);
								fprintf(logTarget, ", at ");
								printDetails(logTarget, details);
								fprintf(logTarget, ". Using \"" NORMAL_GREEN "%s" RESET_STYLE "\"\n", DEFAULT_EXPORT_PATH);
								break;
		default:				
								fprintf(logTarget, "Generic info in ");
								printFunctionName(logTarget, functionName);
								fprintf(logTarget, ", ");
								printDetails(logTarget, details);
								fprintf(logTarget, ", %li]\n", value);
								break;
	}

	fprintf(logTarget, "\n");
}

#define ALL_ERROR_THRESHOLD_FLAGS (THRES_MAE | THRES_MRE | THRES_MSE | THRES_RMSE)
#define ERROR_THRESHOLD_NAME_COUNT 4
const char *ErrorThresholdName[ERROR_THRESHOLD_NAME_COUNT] = {
	"MAE", "MSE", "MRE", "RMSE"
};

void handleLogSingularTestType(FILE *logTarget, SingularTest *test){
	if(test == NULL || logTarget == NULL){
		return;
	}

	uint8_t flags = 0;
	
	fprintf(logTarget, "           ");
	switch(test->type){
		case TYPE_UNDEFINED:	fprintf(logTarget, "type: " BOLD_RED "UNDEFINED" RESET_STYLE "\n");
								break;
		case TYPE_NUMERIC:		fprintf(logTarget, "type: " NORMAL_CYAN "numeric" RESET_STYLE "\tThreshold: " NORMAL_MAGENTA);
								flags = getSingularTestErrorFlag(test, ALL_ERROR_THRESHOLD_FLAGS);
								for(uint8_t i = 0; i < ERROR_THRESHOLD_NAME_COUNT; i++){
									if(!(flags & (1 << i))){
										continue;
									}
									fprintf(logTarget, "%s ", ErrorThresholdName[i]);
								}
								fprintf(logTarget, RESET_STYLE "\n");

								break;
		case TYPE_BOOLEAN:		fprintf(logTarget, "type: " NORMAL_YELLOW "boolean" RESET_STYLE "\n");
								break;
	}

}

void handleLogSingularTestStatusName(FILE *logTarget, SingularTest *test){
	if(test == NULL || logTarget == NULL){
		return;
	}

	switch(test->status){
		case STATUS_NOT_TESTED: fprintf(logTarget, "[" BOLD_MAGENTA "UNTESTED" RESET_STYLE "] ");
								break;
		case STATUS_FAILED: 	fprintf(logTarget, "[" BOLD_RED     " FAILED " RESET_STYLE "] ");
								break;
		case STATUS_OK: 		fprintf(logTarget, "[" BOLD_YELLOW  "   OK   " RESET_STYLE "] ");
								break;
		case STATUS_PASSED: 	fprintf(logTarget, "[" BOLD_GREEN   " PASSED " RESET_STYLE "] ");
								break;
		case STATUS_TESTED: 	fprintf(logTarget, "[" BOLD_CYAN    " TESTED " RESET_STYLE "] ");
								break;
		default:				fprintf(logTarget, "[" BOLD_BLACK   " UNDEF  " RESET_STYLE "] ");
								break;
	}

	fprintf(logTarget, "Test: %s\n", test->name);
}


void printLogSingularTestCommonInfo(FILE *logTarget, SingularTest *test){
	if(test == NULL || logTarget == NULL){
		return;
	}

	fprintf(logTarget, "           IterCount: %li -> %li/%li (%.2lf%%)\n",
		test->iterCount,
		test->passCount,
		test->iterCount,
		(test->passCount / (double) test->iterCount) * 100
	);

	if(test->time.elapsed > 0){
		fprintf(logTarget, "           Time elapsed | %.4le [s]\tAverage: %.4le [s/test]\tMin: %.4le [s]\tMax: %.4le [s]\n",
			test->time.elapsed,
			test->time.average,
			test->time.minValue,
			test->time.maxValue
		);
	}

}

void printLogSingularTestWorstResults(FILE *logTarget, SingularTest *test){
	if(test == NULL || logTarget == NULL){
		return;
	}

	fprintf(logTarget, "           Worst results:\n");
	fprintf(logTarget, "           Index\tArgument:\tExpected Value:\t\tObtained Result:\tError:\n");
	for(uint64_t i = 0; i < WORST_RESULT_COUNT; i++){
		if(test->worstresults[i] == WORST_UNSET){
			continue;
		}

		fprintf(logTarget, "           %li\t\t%lf\t%lf\t\t%lf\t\t", test->worstresults[i], test->arguments[test->worstresults[i]], test->expectedResults[test->worstresults[i]], test->results[test->worstresults[i]]);

		switch(test->worstCriteria){
			case WORST_AE:	fprintf(logTarget, "%lf", calculateAbsoluteError(test->expectedResults[test->worstresults[i]], test->results[test->worstresults[i]]));
								break;
			case WORST_SE:	fprintf(logTarget, "%lf", calculateSquaredError(test->expectedResults[test->worstresults[i]], test->results[test->worstresults[i]]));
								break;
			case WORST_RE:	fprintf(logTarget, "%lf", calculateRelativeError(test->expectedResults[test->worstresults[i]], test->results[test->worstresults[i]]));
								break;
		}

		fprintf(logTarget, "\n");
	}
}

#define PRINT_NUMERIC_DETAILS_ERROR_METRIC(logTarget, test, method, name) do{	\
	fprintf(logTarget, "           %s\t| MAE: %.4lf\tMRE: %.4lf\tMSE: %.4lf\tRMSE: %.4lf\n",\
		name,				\
		test->mae.method,	\
		test->mre.method,	\
		test->mse.method,	\
		test->rmse.method	\
	);						\
} while(0)

void printLogSingularTestNumeric(FILE *logTarget, SingularTest *test){
	if(test == NULL || logTarget == NULL){
		return;
	}

	fprintf(logTarget, "\n");

	PRINT_NUMERIC_DETAILS_ERROR_METRIC(logTarget, test, value, 		"result");
	PRINT_NUMERIC_DETAILS_ERROR_METRIC(logTarget, test, threshold, 	"threshold");
	PRINT_NUMERIC_DETAILS_ERROR_METRIC(logTarget, test, minValue, 	"Min\t");
	PRINT_NUMERIC_DETAILS_ERROR_METRIC(logTarget, test, maxValue, 	"Max\t");

	fprintf(logTarget, "\n");

	printLogSingularTestWorstResults(logTarget, test);

}

ErrorCode logSingularTest(SingularTest *test){
	FILE *logTarget = globalLogTarget;

	if(logTarget == NULL){
		logTarget = stdout;
	}

	if(test == NULL){
		return ERROR_NULL_POINTER;
	}

	handleLogSingularTestStatusName(logTarget, test);
	handleLogSingularTestType(logTarget, test);
	printLogSingularTestCommonInfo(logTarget, test);

	switch(test->type){
		case TYPE_NUMERIC:	printLogSingularTestNumeric(logTarget, test);
							break;
	}

	fprintf(logTarget, "\n");

	return NO_ERROR;
}

ErrorCode logSingularTestHistogram(SingularTest *test, uint8_t type){
	if(test == NULL){
		logError(ERROR_NULL_POINTER, "logSingularTestHistogram", "test", 0);
		return ERROR_NULL_POINTER;
	}

	if(test->results == NULL || test->expectedResults == NULL){
		logError(ERROR_NULL_POINTER, "logSingularTestHistogram", "test->expectedResults or test->results", 0);
		return ERROR_NULL_POINTER;
	}

	size_t chunk = sizeof(double) * test->iterCount;
	double *src = (double *)malloc(chunk);
	if(src == NULL){
		return ERROR_MALLOC;
	}

	double reference = 0;
	double obtained  = 0;
	char *name = NULL;
	ErrorCode r = NO_ERROR;

	for(uint64_t i = 0; i < test->iterCount; i++){
		reference = test->expectedResults[i];
		obtained  = test->results[i];

		switch(type){
			case HISTOGRAM_MAE:	src[i] = calculateAbsoluteError(reference, obtained);
								name = "Absolute Error";
								break;
			case HISTOGRAM_MSE:	src[i] = calculateSquaredError(reference, obtained);
								name = "Squared Error";
								break;
			case HISTOGRAM_MRE:	src[i] = calculateRelativeError(reference, obtained);
								name = "Relative Error";
								break;
			case HISTOGRAM_RMSE:src[i] = sqrt(calculateSquaredError(reference, obtained));
								name = "Square root of error";
								break;
		}
	}
	
	switch(type){
		case HISTOGRAM_MAE:	name = "Absolute Error";
							break;
		case HISTOGRAM_MSE:	name = "Squared Error";
							break;
		case HISTOGRAM_MRE:	name = "Relative Error";
							break;
		case HISTOGRAM_RMSE:name = "Square root of error";
							break;
	}
	
	r = plotHistogram(src, test->iterCount, DEFAULT_X_LENGTH, DEFAULT_Y_LENGTH, name);
	if(r != NO_ERROR) return r;

	free(src);
	return NO_ERROR;
}
