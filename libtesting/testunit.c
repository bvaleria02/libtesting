#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libtesting.h"
#include "enums.h"
#include "types.h"
#include "logger.h"
#include "csv.h"

ErrorCode zeroTestUnit(TestUnit *unit){
	if(unit == NULL){
		return ERROR_NULL_POINTER;
	}

	if(unit->tests == NULL){
		return ERROR_NULL_POINTER;
	}

	if(unit->flags == NULL){
		return ERROR_NULL_POINTER;
	}

	unit->exportPath = NULL;

	for(uint64_t i = 0; i < unit->testCount; i++){
		unit->tests[i] = NULL;
	}

	for(uint64_t i = 0; i < unit->testCount; i++){
		unit->flags[i] = 0;
	}

	return NO_ERROR;
}

TestUnit *createTestUnit(uint64_t count){
	if(count == 0){
		logError(ERROR_ZERO_LENGTH, "createTestUnit", NULL, 0);
		return NULL;
	}

	size_t chunk1 = sizeof(TestUnit);

	TestUnit *unit = (TestUnit *)malloc(chunk1);
	if(unit == NULL){
		logError(ERROR_MALLOC, "createTestUnit", "unit", chunk1);
		return NULL;
	}

	unit->testCount = count;

	size_t chunk2 = sizeof(SingularTest *) * count;

	unit->tests = (SingularTest **)malloc(chunk2);
	if(unit->tests == NULL){
		logError(ERROR_MALLOC, "createTestUnit", "unit->tests", chunk2);
		goto cleanup;
	}

	size_t chunk3 = sizeof(uint8_t *) * count;

	unit->flags = (uint8_t *)malloc(chunk3);
	if(unit->flags == NULL){
		logError(ERROR_MALLOC, "createTestUnit", "unit->flags", chunk3);
		goto cleanup;
	}

	ErrorCode r = NO_ERROR;
	r = zeroTestUnit(unit);
	if(r != NO_ERROR) goto cleanup;

	return unit;
cleanup:
	SAFE_FREE(unit->exportPath);
	SAFE_FREE(unit->flags);
	SAFE_FREE(unit->tests);
	SAFE_FREE(unit);
	return NULL;
}

ErrorCode setFlagTestUnit(TestUnit *unit, uint16_t index, uint8_t flag, uint8_t value){
	if(unit == NULL){
		logError(ERROR_NULL_POINTER, "setFlagTestUnit", "unit", 0);
		return ERROR_NULL_POINTER;
	}

	if(unit->flags == NULL){
		logError(ERROR_NULL_POINTER, "setFlagTestUnit", "unit->flags", 0);
		return ERROR_NULL_POINTER;
	}

	if(index >= unit->testCount){
		logError(ERROR_INDEX_TOOBIG, "setFlagTestUnit", "index", 0);
		return ERROR_NULL_POINTER;
	}
	
	unit->flags[index] = unit->flags[index] & ~(flag);
	if(value & 0x1){
		unit->flags[index] = unit->flags[index] | (flag);
	}

	return NO_ERROR;
}

ErrorCode setFlagAllTestUnit(TestUnit *unit, uint8_t flag, uint8_t value){
	if(unit == NULL){
		logError(ERROR_NULL_POINTER, "setFlagAllTestUnit", "unit", 0);
		return ERROR_NULL_POINTER;
	}

	if(unit->flags == NULL){
		logError(ERROR_NULL_POINTER, "setFlagAllTestUnit", "unit->flags", 0);
		return ERROR_NULL_POINTER;
	}

	ErrorCode r = NO_ERROR;
	for(uint16_t i = 0; i < unit->testCount; i++){
		r = setFlagTestUnit(unit, i, flag, value);
		if(r != NO_ERROR) return r;
	}

	return NO_ERROR;
}

ErrorCode destroyTestUnit(TestUnit *unit){
	if(unit == NULL){
		return ERROR_NULL_POINTER;
	}

	for(uint64_t i = 0; i < unit->testCount; i++){
		if(unit->tests == NULL){
			break;
		}

		destroySingularTest(unit->tests[i]);
	}

	SAFE_FREE(unit->exportPath);
	SAFE_FREE(unit->flags);
	SAFE_FREE(unit->tests);
	SAFE_FREE(unit);

	return NO_ERROR;
}

ErrorCode insertTestIntoTestUnit(TestUnit *unit, SingularTest *test, uint64_t pos){
	if(unit == NULL || test == NULL){
		logError(ERROR_NULL_POINTER, "insertTestIntoTestUnit", "TestUnit or SingularTest", 0);
		return ERROR_NULL_POINTER;
	}
	
	if(pos >= unit->testCount){
		logError(ERROR_INDEX_TOOBIG, "insertTestIntoTestUnit", "pos", 0);
		return ERROR_NULL_POINTER;
	}

	ErrorCode r = NO_ERROR;
	if(unit->tests[pos] != NULL){
		r = destroySingularTest(unit->tests[pos]);
		if(r != NO_ERROR) return r;
	}

	unit->tests[pos] = test;
	return NO_ERROR;
}

ErrorCode TestUnitRun(TestUnit *unit){
	if(unit == NULL){
		logError(ERROR_NULL_POINTER, "TestUnitRun", "unit", 0);
		return ERROR_NULL_POINTER;
	}

	char name[128];
	ErrorCode r = NO_ERROR;
	
	for(uint64_t i = 0; i < unit->testCount; i++){
		if(unit->tests[i] == NULL){
			continue;
		}

		if(unit->flags[i] & UNITFLAG_SKIP_TEST){
			logInfo(INFO_SKIPPED_TEST, "TestUnitRun", unit->tests[i]->name, i+1);
			continue;
		}

		r = SingularTestRun(unit->tests[i]);
		if(r != NO_ERROR) return r;


		if(unit->flags[i] & UNITFLAG_LOG_RESULTS){
			r = logSingularTest(unit->tests[i]);
			if(r != NO_ERROR) return r;
		}

		if(unit->flags[i] & UNITFLAG_SHOW_HISTO_MAE){
			r = logSingularTestHistogram(unit->tests[i], HISTOGRAM_MAE);
			if(r != NO_ERROR) return r;
		}

		if(unit->flags[i] & UNITFLAG_SHOW_HISTO_MRE){
			r = logSingularTestHistogram(unit->tests[i], HISTOGRAM_MRE);
			if(r != NO_ERROR) return r;
		}

		if(unit->flags[i] & UNITFLAG_SHOW_HISTO_MSE){
			r = logSingularTestHistogram(unit->tests[i], HISTOGRAM_MSE);
			if(r != NO_ERROR) return r;
		}

		if(unit->flags[i] & UNITFLAG_EXPORT_CSV){
			memset(name, 0, 128);
			if(unit->exportPath != NULL){
				snprintf(name, 128, "%s%04li.csv", unit->exportPath, i+1);
			} else {
				logInfo(INFO_EXPORT_PATH_UNSET, "TestUnitRun", "unit->exportPath", 0);
				snprintf(name, 128, "%s%04li.csv", DEFAULT_EXPORT_PATH, i+1);
			}
			r = exportSingularTestCSV(unit->tests[i], name);
			if(r != NO_ERROR) return r;
		}

	}

	return NO_ERROR;
}


ErrorCode setExportPathHandler(TestUnit *unit, char *path){
	if(unit == NULL || path == NULL){
		return ERROR_NULL_POINTER;
	}
	
	SAFE_FREE(unit->exportPath);

	size_t nameSize 	= strlen(path) + 1;
	unit->exportPath	= (char *)malloc(nameSize);
	if(unit->exportPath == NULL){
		goto cleanup;
	}
	
	strncpy(unit->exportPath, path, nameSize);
	unit->exportPath[nameSize - 1] = '\0';
	return NO_ERROR;

cleanup:
	SAFE_FREE(unit->exportPath);
	return ERROR_MALLOC;
}

ErrorCode testUnitSet(TestUnit *unit, uint8_t parameter, AbstractTestType value){
	if(unit == NULL){
		return ERROR_NULL_POINTER;
	}

	ErrorCode r = NO_ERROR;
	switch(parameter){
		case TU_EXPORTPATH:	r = setExportPathHandler(unit, value.pointer);
							return r;
	}

	return NO_ERROR;
}
