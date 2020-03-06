/*
 * TFXHelperTimer.c - Test Timer Helper Functions
 *
 * Copyright(c) 2009 MDS Technology Co.,Ltd.
 * All rights reserved.
 *
 * This software contains confidential information of MDS Technology Co.,Ltd.
 * and unauthorized distribution of this software, or any portion of it, are
 * prohibited.
 */
#include <NEOS178S.h>
#include <TFX/TFX.h>
#include <TFX/TFXTimer.h>
#include <asp/powerpc/cpu/mpc567x.h>

#define INTC_BASE_ADDR         0xFFF48000
#define INTC_EOIR(base)     ((v_int *)((base) + 0x0018))         /* INTC end-of-interrupt register */

/* Global Variables */

/**
 * TFXTimerInit - Timer device driver init fucntion
 *
 * DESCRIPTION
 *  This routine initializes a timer device.
 *
 * PARAMETERS
 *  timerId          - [IN] ID of initialized timer device
 *
 * TRACEABILITY
 *  N/A
 *
 * RETURNS
 *   This routine always returns SUCCESS.
 *
 * ERRORS
 *  N/A
 */

Status
TFXTimerInit(Value timerId)
{
    return (SUCCESS);
}

/**
 * TFXTimerOpen - Timer device driver open fucntion
 *
 * DESCRIPTION
 *  This routine opens a timer device specified by name and returns its device 
 *  identifier to id parameter.
 *
 * PARAMETERS
 *  timerId        - [OUT] Device ID of opened timer device
 *  flags          - [IN]  Device access mode
 *
 * TRACEABILITY
 *  N/A
 *
 * RETURNS
 *   This routine always returns SUCCESS.
 *
 * ERRORS
 *  N/A
 */

Status
TFXTimerOpen(Value timerId, Value flags)
{
    return (SUCCESS);
}

/**
 * TFXTimerControl - Timer device driver control fucntion
 *
 * DESCRIPTION
 *  This function performs a variety of control functions on the timer device 
 *  specified by id. The command argument selects the control function to 
 *  be performed and depends on the device being addressed. The data argument
 *  represents additional information that is needed to perform the requested
 *  command. The type of data depends upon the particular control request, but
 *  it is either an integer or a pointer to a device-specific data structure. 
 *
 * PARAMETERS
 *  timerId     - [IN] Identifier of device to control
 *  command     - [IN] Device control command
 *  args        - [IN] Additional information. It is needed to perform the 
 *                     requested command
 *
 * TRACEABILITY
 *  N/A
 *
 * RETURNS
 *   This routine always returns SUCCESS.
 *
 * ERRORS
 *  N/A
 */

Status
TFXTimerControl(Value timerId, Value command, Address args)
{
    unsigned int period  = ((BspGetSysClkRate() / 2) / 1000) * (unsigned int)args;

    switch(command) 
    {
    case TFX_TIMER_START:
        *INTC_EOIR(INTC_BASE_ADDR) = 0x0;
	BspRegWrite32(PIT_RTI_BASE_ADDR + PIT_CH0_LDVAL + (timerId * 0x10), period);
	BspRegWrite32(PIT_RTI_BASE_ADDR + PIT_CH0_TCTRL + (timerId * 0x10), 0x03);
        break;

    case TFX_TIMER_STOP:
	BspRegWrite32(PIT_RTI_BASE_ADDR + PIT_CH0_TCTRL + (timerId * 0x10), 0x0);
	BspRegWrite32(PIT_RTI_BASE_ADDR + PIT_CH0_TFLAG, 0x01);
        break;
    default:
        return (INVALID_PARAM);
    }

    return (SUCCESS);
}

/**
 * TFXTimerClose - Timer device driver close fucntion
 *
 * DESCRIPTION
 *  This function closes a timer device specified by id.
 *
 * PARAMETERS
 *  timerId          - [IN]    Device ID to close
 *
 * TRACEABILITY
 *  N/A
 *
 * RETURNS
 *   This routine always returns SUCCESS.
 *
 * ERRORS
 *  N/A
 */

Status
TFXTimerClose(Value timerId)
{
    return (SUCCESS);
}

