#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "libtesting/libtesting.h"
#include "libtesting/types.h"
#include "libtesting/enums.h"
#include "libtesting/logger.h"


// Test function for test1
double testFunction(double *arguments, uint64_t index){
	double value = arguments[index];
	return value;
}

int main(){
	// Create a test unit with 1 test
	TestUnit *unit = createTestUnit(1);
	
	// Set flags for all tests
	setFlagAllTestUnit(unit, UNITFLAG_SKIP_TEST,      0);	// Don't skip test
	setFlagAllTestUnit(unit, UNITFLAG_LOG_RESULTS,    1);	// Show the results
	setFlagAllTestUnit(unit, UNITFLAG_SHOW_HISTO_MAE, 1);	// Show MAE histogram
	setFlagAllTestUnit(unit, UNITFLAG_SHOW_HISTO_MRE, 1);	// Show MRE histogram
	setFlagAllTestUnit(unit, UNITFLAG_SHOW_HISTO_MSE, 0);	// Don't show MSE histogram
	setFlagAllTestUnit(unit, UNITFLAG_EXPORT_CSV, 	  1);   // export csv with arguments and result

	//testUnitSet(unit, TU_EXPORTPATH, ATT_PTR("output/test"));

	// Create a test
	SingularTest *test1 = createNewSingularTest();

	// Set parameters for tests
	singularTestSet(test1, ST_NAME,				ATT_PTR("Test, \"test\""));
	singularTestSet(test1, ST_TYPE,				ATT_UINT(TYPE_NUMERIC));
	singularTestSet(test1, ST_ITERCOUNT,		ATT_UINT(1024));
	singularTestSet(test1, ST_THRESHOLDFLAG,	ATT_UINT(THRES_MAE | THRES_MRE | THRES_MSE));
	singularTestSet(test1, ST_MAETHRESHOLD,		ATT_FLT(2));
	singularTestSet(test1, ST_MRETHRESHOLD,		ATT_FLT(0.1));
	singularTestSet(test1, ST_MSETHRESHOLD,		ATT_FLT(5));
	singularTestSet(test1, ST_RMSETHRESHOLD,	ATT_FLT(4));
	singularTestSet(test1, ST_PASSTHRESHOLD,	ATT_FLT(0.8));
	singularTestSet(test1, ST_WORSTCRITERIA,	ATT_UINT(WORST_RE));
	singularTestSet(test1, ST_TESTFUNCTION,	    ATT_PTR(testFunction));
	
	// Fill the arguments and expected results
	for(uint64_t i = 0; i < test1->iterCount; i++){
		test1->arguments[i] 		= (double) i;
		test1->expectedResults[i] 	= (double) i + 1;
	}

	// Insert test1 into unit with position 0
	insertTestIntoTestUnit(unit, test1, 0);

	// Run all tests
	TestUnitRun(unit);

	// Free both test unit and singular tests
	destroyTestUnit(unit);
	return 0;
}
