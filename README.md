# libtesting
C library for atomatic numeric testing and automation of tests.

## How it works?
libtesting is aimed to eval implementations of algorythms, like comparing CORDIC or Taylor implementation of sine to the one in math.h.

This include:
* MAE (Median Absolute Error), MRE (Mean Relative Error), MSE (Mean Squared Error), RMSE (Root Mean Squared Error).
* Minimum value and maximum for average error, relative error and squared error.
* Time (min, max, average, elapsed).
* Histogram for absolute, relative and squared error.
* CSV export
* A table for worst cases (can use absolute, relative or squared error as metric).
* Terminal logging with colors.

## Setup
This project is aimed to work in Linux, for now Windows support is not ready. 

To compile the example test: `make`

Or, you can compile and run it with valgrind: `make memtest`

As for integrating into another project, compile all the `.c ` under `/libtesting` and link to your code

## Simple example (the one in main.c)
Include these into your project
```C
#include "libtesting/libtesting.h"     // Function prototypes
#include "libtesting/types.h"          // Types (SingularTest, TestUnit)
#include "libtesting/enums.h"          // Enums to configure the test behavior
#include "libtesting/logger.h"         // Logging module
#include "libtesting/timer.h"          // Timer (only if you want to time your code)
```

Create a **TestUnit**, a container with tests, and also where you can configure the output, disable tests, and also export to CSV. The parameter of createTestUnit is the amount of individual tests, in this case, 1.
```C
TestUnit *unit = createTestUnit(1);
```

Configure the **behavior** of the test container. To set **all** the individual tests with the same settings, use setFlagsAllTestUnit:
```C
setFlagAllTestUnit(unit, flag, bool);
```
else, use setFlagTestUnit:
```C
setFlagTestUnit(unit, index, flag, bool);
```

As for flags, use:
* `UNITFLAG_SKIP_TEST` to **skip** the test
* `UNITFLAG_SHOW_RESULTS` to **show** the results of the test after running.
* `UNITFLAG_SHOW_HISTO_MAE` shows an histogram of the **Absolute Error**. You can also use `UNITFLAG_SHOW_HISTO_MRE`for **Relative Error** and `UNITFLAG_SHOW_HISTO_MSE` for **Squared Error**.
* `UNITFLAG_EXPORT_CSV` to **export** the arguments, results and expected results into a csv.

To change the directory of the .csv files, use
```C
testUnitSet(unit, TU_EXPORTPATH, ATT_PTR(path));
```
**NOTE:** "path" can be the suffix of the file (**test**0001.csv) or the path (**output/test**0001.csv). All files are filled with XXXX.csv, with XXXX being the test position. If no name is set, it uses the default path (./test). Also, ATT_PTR is a macro to convert pointers into AbstractTestType and **it is needed**.

Now, lets create a blank test. No arguments needed
```C
SingularTest *test1 = createNewSingularTest();
```

Now you can configure it with setSingularTest:
```C
singularTestSet(test, flag, value);
```

With flags being:
* `ST_NAME` to set the **name** of the test, for example `ATT_PTR("CORDIC sin for fixed point")`
* `ST_TYPE` to choose between **TYPE_NUMERIC** (Compares if the values are within a tolerance), or **TYPE_BOOLEAN** (they need to be the exact value). Use it with `ATT_UINT(TYPE_NUMERIC)`
* `ST_ITERCOUNT` to set the amount of iterations to run the test. For example `ATT_UINT(1024)`
* `ST_THRESHOLDFLAG` (numeric type only) to set the metric (MAE, MSE, MRE) to compare the output to mark it as passed or not. Use it like `ATT_UINT(THRES_MAE | THRES_MSE | THRES_MRE)`
* `ST_MAETHRESHOLD`, `ST_MRETHRESHOLD` and `ST_MSETHRESHOLD` for the threshold of the metric to discard/accept a value. Use it with value `ATT_FLT(1.0)`
* `ST_PASSTHRESHOLD` to select the threshold to mark the **entire** test as passed (in a range between 0 and 1, with 1 being 100% to mark it as passed). Use the value `ATT_FLT(0.8)`
* `ST_WORSTCRITERIA` to set the criteria for the **worst** cases to show in the summary, set it with `ATT_UINT(criteria)`, with criteria being `WORST_AE` for Absolute Error, `WORST_RE` for Relative Error and `WORST_SE` for Squared Error.
* `ST_TESTFUNCTION` to link the test function. Use it like `ATT_PTR(myTestFunction)`.

You can also copy tests using
```C
copyDetailsSingularTest(destination, source);
```

To run all tests, just use
```C
TestUnitRun(unit);
```

Depending on the flags you set, you will get a terminal output, a .csv file, or nothing (if you leave everything without configure).

Here is an example of the following set:
```C
setFlagAllTestUnit(unit, UNITFLAG_SKIP_TEST,      0);	// Don't skip test
setFlagAllTestUnit(unit, UNITFLAG_LOG_RESULTS,    1);	// Show the results
setFlagAllTestUnit(unit, UNITFLAG_SHOW_HISTO_MAE, 1);	// Show MAE histogram
setFlagAllTestUnit(unit, UNITFLAG_SHOW_HISTO_MRE, 1);	// Show MRE histogram
setFlagAllTestUnit(unit, UNITFLAG_SHOW_HISTO_MSE, 0);	// Don't show MSE histogram
setFlagAllTestUnit(unit, UNITFLAG_EXPORT_CSV, 	  1);   // export csv with arguments and result
```

![Screenshot of the output](https://github.com/bvaleria02/libtesting/blob/main/assets/1.png?raw=true)

And here is the CSV output, it includes the index, arguments, the expected results (for example from math.h sin) and the results (for example your CORDIC implementation of sin).

![Screenshot of the CSV](https://github.com/bvaleria02/libtesting/blob/main/assets/2.png?raw=true)
