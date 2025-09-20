#ifndef LIBTESTING_H
#define LIBTESTING_H

#include "types.h"
#include "enums.h"

#define SAFE_FREE(x) do{ 	\
	if((x) != NULL){			\
		free((x));				\
		(x) = NULL;				\
	}							\
} while(0)

#define DEFAULT_EXPORT_PATH "./test"

// src/singulartest.c
SingularTest *createNewSingularTest();
void singularTestSet(SingularTest *test, uint8_t parameter, AbstractTestType value);
ErrorCode destroySingularTest(SingularTest *test);
ErrorCode SingularTestRun(SingularTest *test);
ErrorCode SingularTestZeroAll(SingularTest *test);
void copyDetailsSingularTest(SingularTest *dest, SingularTest *src);
uint8_t getSingularTestErrorFlag(SingularTest *test, uint8_t flag);
ErrorCode singularTestSetPointer(SingularTest *test, uint8_t parameter, void *value);
ErrorCode singularTestSetFloat(SingularTest *test, uint8_t parameter, double value);
ErrorCode singularTestSetInt(SingularTest *test, uint8_t parameter, int64_t value);

// src/testunit.c
TestUnit *createTestUnit(uint64_t count);
ErrorCode destroyTestUnit(TestUnit *unit);
ErrorCode insertTestIntoTestUnit(TestUnit *unit, SingularTest *test, uint64_t pos);
ErrorCode TestUnitRun(TestUnit *unit);
ErrorCode setFlagTestUnit(TestUnit *unit, uint16_t index, uint8_t flag, uint8_t value);
ErrorCode setFlagAllTestUnit(TestUnit *unit, uint8_t flag, uint8_t value);
ErrorCode testUnitSet(TestUnit *unit, uint8_t parameter, AbstractTestType value);

#endif // LIBTESTING_H
