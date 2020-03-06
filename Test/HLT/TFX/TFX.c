/*
 * TFX.c - Test framework
 *
 * Copyright(c) 2009 MDS Technology Co.,Ltd.
 * All rights reserved.
 *
 * This software contains confidential information of MDS Technology Co.,Ltd.
 * and unauthorized distribution of this software, or any portion of it, are
 * prohibited.
 */

#include <TFX/TFX.h>
#include <NEOS178S.h>

/* holds the failing line number. */
int    testExitLineNumber = 0;
int    testExitStatus     = FAILURE;
static Thread mainId;

/**
 * TFXInit - Initialize TFX and print test start delemeter
 *
 * DESCRIPTION
 *  This function initializes the test framework and print test start
 *  delemeter to the console.
 *
 * PARAMETERS
 *  N/A
 *
 * TRACEABILITY
 *  N/A
 *
 * RETURNS
 *  This routine returns SUCCESS if the function initializes TFX successfully,
 *  or it returns ERROR to indicate the error.
 *
 * ERRORS
 *  N/A
 */
static Status
TFXInit(void)
{
    printk("\n[[TFXStart]]\n");
    return (SUCCESS);
}

/**
 * TFXPrintTestId - Print test id to the console.
 *
 * DESCRIPTION
 *  This function prints out the test ID to the console.
 *
 * PARAMETERS
 *  N/A
 *
 * TRACEABILITY
 *  N/A
 *
 * RETURNS
 *  N/A
 *
 * ERRORS
 *  N/A
 */
static void
TFXPrintTestId(TEST_ID *testId)
{
    printk("\n[[TFXTestName]] ");
    printk("  name:    %s\n", testId->tctName);
    printk("\n[[TFXTestVersion]] ");
    printk("  version: %s\n", testId->testVersion);
}

/**
 * TFXPrintTestResult - Print test result to the console
 *
 * DESCRIPTION
 *  This function prints out the test message to the console.
 *
 * PARAMETERS
 *  N/A
 *
 * TRACEABILITY
 *  N/A
 *
 * RETURNS
 *  N/A
 *
 * ERRORS
 *  N/A
 */
void 
TFXPrintTestResult(Status theResult)
{
    printk("\n[[TFXTestResult]] ");
    if (testExitStatus == SUCCESS)
    {
        printk("SUCCESS %d %d\n", theResult, testExitLineNumber);
    }
    else
    {
        printk("FAILURE %d %d\n", theResult, testExitLineNumber);
    }
    testExitLineNumber = 0;
    testExitStatus     = FAILURE;
}

/**
 * TFXPrintCoverageData - Print structural coverage data
 *
 * DESCRIPTION
 *  This function prints out the test coverage message to the console.
 *
 * PARAMETERS
 *  N/A
 *
 * TRACEABILITY
 *   N/A
 *
 * RETURNS
 *  N/A 
 *
 * ERRORS
 *  N/A
 */
void
TFXPrintCoverageData(void)
{
    /* Nothing */
}

/**
 * TFXExit - Terminate the test framework
 *
 * DESCRIPTION
 *  This function prints out test end delemeter.
 *
 * PARAMETERS
 *  N/A
 *
 * TRACEABILITY
 *  N/A
 *
 * RETURNS
 *  N/A
 *
 * ERRORS
 *  N/A
 */
void
TFXExit(void)
{
    printk("\n[[TFXExit]]\n");

    TFXDelayByBusyLoop(100000000ULL);
    TFXReset();
}

/**
 * TFXTct - Entry point for the tet context thread
 *
 * DESCRIPTION
 *  Entry point for the test context thread. It invSUCCESSes user test code and
 *  prints out the test result and code structural coverage result.
 *
 * PARAMETERS
 *  N/A
 *
 * TRACEABILITY
 *  N/A
 *
 * RETURNS
 *  NULL to indicate that this function is performed correctly.
 *
 * ERRORS
 *  N/A
 */
static void *
TFXTct(void *args)
{
    Status result = SUCCESS;
    TEST_ID * testId = (TEST_ID *) args;

    /* suspend main thread until we are done */
    ThreadSuspend(mainId);

    /* InvSUCCESSe test function */
    printk(" TCT B doTest\n");
    result = testId->doTest();

    /* Cleanup the test */
    printk(" TCT B cleanUp\n");
    testId->cleanUp();

    /* Print out test result that can be "FALURE" or "PASSED" */
    TFXPrintTestResult(result);

    /* resume main thread: we are done */
    ThreadResume(mainId);

    return (NULL);
}

/**
 * TFXMain - Entry of TFX
 *
 * DESCRIPTION
 *  Main function of TFX. This function should be invSUCCESSed in the system initialization
 *  state in entry point of NEOS-178S
 *
 * PARAMETERS
 *  N/A
 *
 * TRACEABILITY
 *  N/A
 *
 * RETURNS
 *  This routine returns SUCCESS if this function is executed successfully,
 *  or it returns ERROR to indicate the error.
 *
 * ERRORS
 *  N/A
 */
Status
TFXMain(void)
{
    Status result = SUCCESS;
    int i;
    Thread tct[g_TestCount];

    /* Initialize Test Framework and print out test start delemeter */
    TFXInit();

    for(i = 0; i < g_TestCount; i++)
    {
        result = ThreadCreate(g_TestIds[i]->tctName, 4096, 200, (Address)TFXTct, (Address)(g_TestIds[i]), &tct[i]);
    	printk("TFXMAIN_ThreadCreate:result = %d\n", result);
        if(result != SUCCESS)
        {
            /* this is a structural failure, print and exit */
            testExitLineNumber = __LINE__;
            testExitStatus     = FAILURE;
            TFXPrintTestId(g_TestIds[i]);
            TFXPrintTestResult(result);
            TFXPrintCoverageData();
            TFXExit();
        }

        result = g_TestIds[i]->setPreconditions();
        /* if the pre-conditions fail, the test is invalid, so report and exit */
        if(result != SUCCESS)
        {
            /* Print out the test ID (name, version) */
            TFXPrintTestId(g_TestIds[i]);

            /* Print out test result that can be "FALURE" or "PASSED" */
            TFXPrintTestResult(result);

            /* Dump coverage data which vectorCAST rsp accumulated. */
            TFXPrintCoverageData();

            TFXExit();
        }

    }

    SystemStateSetNormal();

    mainId = ThreadGetIdSelf();
    ThreadSetPriority(mainId, 151);

    for(i = 0; i < g_TestCount; i++)
    {
        /* Print out the test ID (name, version) */
        TFXPrintTestId(g_TestIds[i]);

        /* run the test */
        result = ThreadStart(tct[i], 0);
        printk("TFXMAIN_ThreadStart:result = %d\n", result);
    }

    /* Dump coverage data which vectorCAST rsp accumulated. */
    TFXPrintCoverageData();

    TFXExit();

    return (result);
}

/* Entry point of NEOS-178S */
Status 
UserMain(void)
{
    printk("Start UserMain\n");

    /* Do the main thread logic */
    TFXMain();

    return (SUCCESS);
}

