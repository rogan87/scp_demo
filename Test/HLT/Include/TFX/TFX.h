/*
 * TFX.h - Test FrameWork Header File
 *
 * Copyright(c) 2009 MDS Technology Co.,Ltd.
 * All rights reserved.
 *
 * This software contains confidential information of MDS Technology Co.,Ltd.
 * and unauthorized distribution of this software, or any portion of it, are
 * prohibited.
 */

#ifndef _TFX_H
#define _TFX_H

#include <NEOS178STypes.h>
#include "TFXTimer.h"
#include "TFXHelper.h"
#include <asp/powerpc/cpu/mpc567x.h>
#include <drivers/intrctrl/Mpc567xInterrupt.h>

/* Constants for test restuls */
#define SUCCESS     0
#define FAILURE     -1

/* 
 * Constants for timing value.
 *   This constants are used for TimeValue type variabes whose type is unsigned
 *   long long.
 */
#define TEST_NSEC   1               /* nano-second */
#define TEST_USEC   1000            /* micro-second */
#define TEST_MSEC   1000000ULL      /* mili-second */
#define TEST_SEC    1000000000ULL   /* second */

/* Time margine which is used to see if the time is in the range of time. */
#define TFX_TIME_MARGIN          10000000

extern int testExitLineNumber;
extern int testExitStatus;
#define TEST_FAILURE(exitCode) testExitLineNumber = __LINE__; \
                               testExitStatus     = FAILURE;  \
                               return (exitCode);
#define TEST_SUCCESS(exitCode) testExitLineNumber = __LINE__; \
                               testExitStatus     = SUCCESS;  \
                               return (exitCode);

/* 
 * Test ID structure.
 *  tctName:
 *    Test Context Thread Name
 *  testName: 
 *    Test name corresponding test procedure. This field is
 *    automatically filled by Subversion if "$Id: TFX.h 18 2009-07-02 02:06:42Z chayeon $" is assigned for this field.
 *  testVersion: 
 *    Test version number which is Subversion revision number. It
 *    is also automatically filled by Subversion if "$Version$" is assigned for
 *    this field.
 *  setPreconditions: 
 *    Function pointer to TestSetPrecondition() function. In this function, 
 *    there should be code that should be executed in SYSTEM_INITIALIZATION 
 *    state.
 *  doTest: 
 *    Function pointer to TestDoTest() function. In this function, there should
 *    be code that should be executed in SYSTEM_OPERATIONAL state.
 *  cleanUp:
 *    Function pointer to TestCleanUp() function. In this funciton, there 
 *    should be code that should clean up the resources such as threads.
 */
typedef struct
{
    char * tctName;
    char * testName;
    char * testVersion;
    int  (*setPreconditions)(void);
    int  (*doTest)(void);
    void (*cleanUp)(void);
} TEST_ID;

/*
 * Declaration for Test ID array that is used for test suites.
 */
extern TEST_ID * g_TestIds[];
extern int       g_TestCount;

/*
 * Test framework extern function TFXMain() which is the entry point for test
 * framework. 
 */
extern Status TFXMain(void);

/*
 * prototypes for functions that allow individual tests to report results and
 * exit without returning control to the main thread.
 */

void TFXPrintTestResult(Status theResult);
void TFXPrintCoverageData(void);
void TFXExit(void);

#endif /* _TFX_H */
