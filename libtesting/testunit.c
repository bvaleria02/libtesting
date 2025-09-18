#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libtesting.h"
#include "enums.h"
#include "types.h"
#include "logger.h"
#include "csv.h"

void zeroTestUnit(TestUnit *unit){
	if(unit == NULL){
		return;
	}

	if(unit->tests == NULL){
		return;
	}

	if(unit->flags == NULL){
		return;
	}

	unit->exportPath = NULL;

	for(uint64_t i = 0; i < unit->testCount; i++){
		unit->tests[i] = NULL;
	}

	for(uint64_t i = 0; i < unit->testCount; i++){
		unit->flags[i] = 0;
	}
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


	zeroTestUnit(unit);

	return unit;
cleanup:
	SAFE_FREE(unit->exportPath);
	SAFE_FREE(unit->flags);
	SAFE_FREE(unit->tests);
	SAFE_FREE(unit);
	return NULL;
}

void setFlagTestUnit(TestUnit *unit, uint16_t index, uint8_t flag, uint8_t value){
	if(unit == NULL){
		logError(ERROR_NULL_POINTER, "setFlagTestUnit", "unit", 0);
		return;
	}

	if(unit->flags == NULL){
		logError(ERROR_NULL_POINTER, "setFlagTestUnit", "unit->flags", 0);
		return;
	}

	if(index >= unit->testCount){
		logError(ERROR_INDEX_TOOBIG, "setFlagTestUnit", "index", 0);
		return;
	}
	
	unit->flags[index] = unit->flags[index] & ~(flag);
	if(value & 0x1){
		unit->flags[index] = unit->flags[index] | (flag);
	}
}

void setFlagAllTestUnit(TestUnit *unit, uint8_t flag, uint8_t value){
	if(unit == NULL){
		logError(ERROR_NULL_POINTER, "setFlagAllTestUnit", "unit", 0);
		return;
	}

	if(unit->flags == NULL){
		logError(ERROR_NULL_POINTER, "setFlagAllTestUnit", "unit->flags", 0);
		return;
	}

	for(uint16_t i = 0; i < unit->testCount; i++){
		setFlagTestUnit(unit, i, flag, value);
	}
}

void destroyTestUnit(TestUnit *unit){
	if(unit == NULL){
		return;
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
}

void insertTestIntoTestUnit(TestUnit *unit, SingularTest *test, uint64_t pos){
	if(unit == NULL || test == NULL){
		logError(ERROR_NULL_POINTER, "insertTestIntoTestUnit", "TestUnit or SingularTest", 0);
		return;
	}
	
	if(pos >= unit->testCount){
		logError(ERROR_INDEX_TOOBIG, "insertTestIntoTestUnit", "pos", 0);
		return;
	}

	if(unit->tests[pos] != NULL){
		destroySingularTest(unit->tests[pos]);
	}

	unit->tests[pos] = test;
}

void TestUnitRun(TestUnit *unit){
	if(unit == NULL){
		logError(ERROR_NULL_POINTER, "TestUnitRun", "unit", 0);
		return;
	}

	char name[128];
	
	for(uint64_t i = 0; i < unit->testCount; i++){
		if(unit->tests[i] == NULL){
			continue;
		}

		if(unit->flags[i] & UNITFLAG_SKIP_TEST){
			logInfo(INFO_SKIPPED_TEST, "TestUnitRun", unit->tests[i]->name, i+1);
			continue;
		}

		SingularTestRun(unit->tests[i]);

		if(unit->flags[i] & UNITFLAG_LOG_RESULTS){
			logSingularTest(unit->tests[i]);
		}

		if(unit->flags[i] & UNITFLAG_SHOW_HISTO_MAE){
			logSingularTestHistogram(unit->tests[i], HISTOGRAM_MAE);
		}

		if(unit->flags[i] & UNITFLAG_SHOW_HISTO_MRE){
			logSingularTestHistogram(unit->tests[i], HISTOGRAM_MRE);
		}

		if(unit->flags[i] & UNITFLAG_SHOW_HISTO_MSE){
			logSingularTestHistogram(unit->tests[i], HISTOGRAM_MSE);
		}

		if(unit->flags[i] & UNITFLAG_EXPORT_CSV){
			memset(name, 0, 128);
			if(unit->exportPath != NULL){
				snprintf(name, 128, "%s%04li.csv", unit->exportPath, i+1);
			} else {
				logInfo(INFO_EXPORT_PATH_UNSET, "TestUnitRun", "unit->exportPath", 0);
				snprintf(name, 128, "%s%04li.csv", DEFAULT_EXPORT_PATH, i+1);
			}
			exportSingularTestCSV(unit->tests[i], name);
		}

	}
}


void setExportPathHandler(TestUnit *unit, char *path){
	if(unit == NULL || path == NULL){
		return;
	}
	
	SAFE_FREE(unit->exportPath);

	size_t nameSize 	= strlen(path) + 1;
	unit->exportPath	= (char *)malloc(nameSize);
	if(unit->exportPath == NULL){
		goto cleanup;
	}
	
	strncpy(unit->exportPath, path, nameSize);
	unit->exportPath[nameSize - 1] = '\0';
	return;

cleanup:
	SAFE_FREE(unit->exportPath);
}

void testUnitSet(TestUnit *unit, uint8_t parameter, AbstractTestType value){
	if(unit == NULL){
		return;
	}

	switch(parameter){
		case TU_EXPORTPATH:	setExportPathHandler(unit, value.pointer);
							break;
	}
}
