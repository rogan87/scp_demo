/*
 * TFXHelper.h - Test Helper Functions Header File
 *
 * Copyright(c) 2009 MDS Technology Co.,Ltd.
 * All rights reserved.
 *
 * This software contains confidential information of MDS Technology Co.,Ltd.
 * and unauthorized distribution of this software, or any portion of it, are
 * prohibited.
 */

#ifndef _TFXHELPER_H_
#define _TFXHELPER_H_

#include <NEOS178STypes.h>
#include <asp/powerpc/ppce200z7/MmuConfig.h>

/*
 * RTC structure in which RTC time value is retrieved.
 */
typedef union _TFXRtc {
    unsigned long long ll;
    unsigned long      l[2];
} TFXRtc;

/* 
 * MMU Map structure in which MMU mapping information is retrieved.
 */
typedef struct _TFXMmuMap {
    Address va;        /* Start address of the virtual memory space */
    Address pa;        /* Start address of the physical memory space */
    Address size;      /* Size of the address space */
    Value attr;        /* Attributes of the space */
} TFXMmuMap;

/* 
 * Helper functions for HLTPs
 */
void TFXSetRTC(unsigned long long rtc);
unsigned long long TFXGetRTC(void);
Value TFXGetDecrementer(void);
void TFXReset(void);
void TFXAsmReset(void);
volatile PTE * TFXFindMmuPte(Address va, Address pa);
int TFXAttachTimer(char *TimerName, Address timerNumber);
extern void TFXDelayByBusyLoop(TimeValue duration);
void TFXGenAlignExc(unsigned int);
void TFXGenVmxAsstExc(long long *);
int TFXMemCompare(const void *, const void *, Size);
void TFXRegisterBspStopCallback(void (*callback)(void));
void TFXRegisterBspResetCallback(void (*callback)(void));

/* 
 * Helper functions for debugging
 */
void printk(char *fmt, ...);

#endif
