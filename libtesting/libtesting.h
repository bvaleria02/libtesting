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
void destroySingularTest(SingularTest *test);
void SingularTestRun(SingularTest *test);
void SingularTestZeroAll(SingularTest *test);
void copyDetailsSingularTest(SingularTest *dest, SingularTest *src);

// src/testunit.c
TestUnit *createTestUnit(uint64_t count);
void destroyTestUnit(TestUnit *unit);
void insertTestIntoTestUnit(TestUnit *unit, SingularTest *test, uint64_t pos);
void TestUnitRun(TestUnit *unit);
void setFlagTestUnit(TestUnit *unit, uint16_t index, uint8_t flag, uint8_t value);
void setFlagAllTestUnit(TestUnit *unit, uint8_t flag, uint8_t value);
void testUnitSet(TestUnit *unit, uint8_t parameter, AbstractTestType value);

#endif // LIBTESTING_H
