/*
 * TFXTimer.h - Test Timer Functions Header File
 *
 * Copyright(c) 2009 MDS Technology Co.,Ltd.
 * All rights reserved.
 *
 * This software contains confidential information of MDS Technology Co.,Ltd.
 * and unauthorized distribution of this software, or any portion of it, are
 * prohibited.
 */

#ifndef _TFXTIMER_H_
#define _TFXTIMER_H_

#include <NEOS178STypes.h>

/* IRQ numbers for timer devices in PA-SEMI */
#define TFX_INTR_TIMER0        (301)
#define TFX_INTR_TIMER1        (302)

/* IOCTL commands for timer devices */
#define TFX_TIMER_START        (0)
#define TFX_TIMER_STOP         (1)
#define TFX_TIMER_PERIOD_SET   (2)


/* Provided Interfaces */
Status TFXTimerInit(Value timerId);
Status TFXTimerOpen(Value timerId, Value flags);
Status TFXTimerControl(Value timerId, Value command, Address args);
Status TFXTimerClose(Value timerId);

#endif    /* !_TFXTIMER_H_ */
