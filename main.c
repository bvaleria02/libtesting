#include <stdio.h>
//#include <stdlib.h>
#include <stdint.h>
#include "libtesting/libtesting.h"
#include "libtesting/types.h"
#include "libtesting/enums.h"
//#include "libtesting/logger.h"
#include "libtesting/timer.h"

// Test function for test1
double testFunction(double *arguments, uint64_t index, double *time){
	double value = arguments[index];

	// Comment below if you don't want to time it
	startTimer();

	// Place here the function you want to time
	value = value * 0.996 - 1;

	// Comment below if you don't want to time it
	(*time) = stopTimer();

	// Uncomment below if you don't want to time it
	// (void) time;

	return value;
}

int main(){
	// Create a test unit with 1 test
	TestUnit *unit = createTestUnit(1);
	if(unit == NULL) return ERROR_MALLOC;
	
	// Error code to check if something failed
	ErrorCode r = NO_ERROR;

	// Set flags for all tests
	r = setFlagAllTestUnit(unit, UNITFLAG_SKIP_TEST,      0);	// Don't skip test
	if(r != NO_ERROR) return r;
	r = setFlagAllTestUnit(unit, UNITFLAG_LOG_RESULTS,    1);	// Show the results
	if(r != NO_ERROR) return r;
	r = setFlagAllTestUnit(unit, UNITFLAG_SHOW_HISTO_MAE, 1);	// Show MAE histogram
	if(r != NO_ERROR) return r;
	r = setFlagAllTestUnit(unit, UNITFLAG_SHOW_HISTO_MRE, 1);	// Show MRE histogram
	if(r != NO_ERROR) return r;
	r = setFlagAllTestUnit(unit, UNITFLAG_SHOW_HISTO_MSE, 0);	// Don't show MSE histogram
	if(r != NO_ERROR) return r;
	r = setFlagAllTestUnit(unit, UNITFLAG_EXPORT_CSV, 	  1);   // export csv with arguments and result
	if(r != NO_ERROR) return r;

	// Set export path. Uncomment to change export path
	/*
	r = testUnitSet(unit, TU_EXPORTPATH, ATT_PTR("output/test"));
	if(r != NO_ERROR) return r;
	*/

	// Create a test
	SingularTest *test1 = createNewSingularTest();
	if(test1 == NULL) return ERROR_MALLOC;

	// Set parameters for tests
	r = singularTestSetPointer(	test1, ST_NAME,				"Test, \"test\"");
	if(r != NO_ERROR) return r;
	r = singularTestSetInt(		test1, ST_TYPE,				TYPE_NUMERIC);
	if(r != NO_ERROR) return r;
	r = singularTestSetInt(		test1, ST_ITERCOUNT,		1024);
	if(r != NO_ERROR) return r;
	r = singularTestSetInt(	test1, ST_THRESHOLDFLAG,	THRES_MAE | THRES_MRE | THRES_MSE);
	if(r != NO_ERROR) return r;
	r = singularTestSetFloat(	test1, ST_MAETHRESHOLD,		2);
	if(r != NO_ERROR) return r;
	r = singularTestSetFloat(	test1, ST_MRETHRESHOLD,		0.1);
	if(r != NO_ERROR) return r;
	r = singularTestSetFloat(	test1, ST_MSETHRESHOLD,		5);
	if(r != NO_ERROR) return r;
	r = singularTestSetFloat(	test1, ST_RMSETHRESHOLD,	4);
	if(r != NO_ERROR) return r;
	r = singularTestSetFloat(	test1, ST_PASSTHRESHOLD,	0.8);
	if(r != NO_ERROR) return r;
	r = singularTestSetInt(		test1, ST_WORSTCRITERIA,	WORST_RE);
	if(r != NO_ERROR) return r;
	r = singularTestSetPointer(	test1, ST_TESTFUNCTION,	    testFunction);
	if(r != NO_ERROR) return r;
	
	// Fill the arguments and expected results
	for(uint64_t i = 0; i < test1->iterCount; i++){
		test1->arguments[i] 		= (double) i;
		test1->expectedResults[i] 	= (double) i;
	}

	// Insert test1 into unit with position 0
	r = insertTestIntoTestUnit(unit, test1, 0);
	if(r != NO_ERROR) return r;

	// Run all tests
	r = TestUnitRun(unit);
	if(r != NO_ERROR) return r;

	// Free both test unit and singular tests
	r = destroyTestUnit(unit);
	if(r != NO_ERROR) return r;

	return 0;
}
