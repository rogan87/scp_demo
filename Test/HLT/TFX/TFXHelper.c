/*
 * TFXHelper.c - Test Helper Functions
 *
 * Copyright(c) 2009 MDS Technology Co.,Ltd.
 * All rights reserved.
 *
 * This software contains confidential information of MDS Technology Co.,Ltd.
 * and unauthorized distribution of this software, or any portion of it, are
 * prohibited.
 */

#include <TFX/TFX.h>
#include <TFX/TFXHelper.h>
#include <asp/powerpc/ppce200z7/MmuConfig.h>
#include <BspConfig.h>

/* RTC union. This type is used only in this file. */
typedef union rtc
{
    unsigned long long ll;
    unsigned long      l[2];
} RTC;

/**
 * PteToVa - Convert a PTE object to the virtual address regarding the PTE.
 *
 * DESCRIPTION
 *  This function converts a PTE object to the virtual address and return the address.
 *
 * PARAMETERS
 *  pt [IN] - Reference to the PTE object which is converted to the virtual address.
 *
 * TRACEABILITY
 *  N/A
 *
 * RETURNS
 *  This routine returns the virtual address regarding the parameter pt.
 *
 * ERRORS
 *  N/A
 */
static Address 
PteToVa(volatile PTE *pt)
{
#if 0
    Address va;
    unsigned long ptaddr = (unsigned long)pt;
    unsigned long srn;
    unsigned long vsid;

    if (pt->pteHi & PTE_HID)
    {
        ptaddr ^= (0x1ffff * sizeof(PTEG));
    }
    /* OEA bits 10-19, OEA64 bits 42-51 */
    va = ((pt->pteHi >> PTE_VSID_SHFT) ^ (ptaddr / sizeof(PTEG))) & 0x7ff;
    va <<= ADDR_PIDX_SHFT;

    /* OEA bits 4-9 OEA64 36-41 */
    va |= (pt->pteHi & PTE_API) << (ADDR_API_SHFT - PTE_API_SHFT);
    vsid = (pt->pteHi) >> 12;
    srn = (vsid & 0xf) << 28;
    va |= srn;

    return (va);
#endif
    return 0;
}

/**
 * PteToPa - Convert a PTE object to the physical address regarding the PTE.
 *
 * DESCRIPTION
 *  This function converts a PTE object to the physical address and return the address.
 *
 * PARAMETERS
 *  pt [IN] - Reference to the PTE object which is converted to the physical address.
 *
 * TRACEABILITY
 *  N/A
 *
 * RETURNS
 *  This routine returns the physical address regarding the parameter pt.
 *
 * ERRORS
 *  N/A
 */
static Address
PteToPa(volatile PTE *pt)
{
#if 0
    return (pt->pteLo & PTE_RPGN);
#endif
    return (0);
}

/**
 * TFXSetRTC - Set RTC value.
 *
 * DESCRIPTION
 *  Nothing to do.
 *
 * PARAMETERS
 *  nsec [IN] - Time value to be set to RTC.
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
TFXSetRTC(unsigned long long nsecs)
{
    return;
}

/**
 * TFXGetRTC - Get RTC value in nano second unit.
 *
 * DESCRIPTION
 *  This function gets RTC value in nano second unit.
 *
 * PARAMETERS
 *  N/A
 *
 * TRACEABILITY
 *  N/A
 *
 * RETURNS
 *  This routine returns the value of RTC in nano second unit.
 *
 * ERRORS
 *  N/A
 */
unsigned long long
TFXGetRTC(void)
{
    RTC tb;
    unsigned long tbu;

    __asm __volatile("1: mftbu %0; mftb %1; mftbu %2; cmpw %0, %2; bne 1b" :
        "=r"(tb.l[0]), "=r"(tb.l[1]), "=r"(tbu));

      return (tb.ll * 1000000000/BspGetSysClkRate());
}

/**
 * TFXReset - Reset target when test complete.
 *
 * DESCRIPTION
 *  This function resets the target board when test complete.
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
TFXReset(void)
{
    /* PowerTrace losts trace data of PowerTrace when system is reset. */
    while(1);
}

/**
 * TFXFindMmuPte - Find a MMU entry from MMU table in the system
 *
 * DESCRIPTION
 *  This function finds a MMU entry from MMU table in the system and returns 
 *  the pointer to the entry.
 *
 * PARAMETERS
 *  va [IN] - Virtual address related to the PTE entry to be found.
 *  pa [IN] - Physical address related to the PTE entry to be vound.
 *
 * TRACEABILITY
 *  N/A
 *
 * RETURNS
 *  This routine returns the PTE entry found.
 *
 * ERRORS
 *  N/A
 */
volatile PTE *
TFXFindMmuPte(Address va, Address pa)
{
#if 0
    extern Address systemMemoryEnd;
    volatile PTE *pt;
    PTEG *ptegTable;
    unsigned long ptegidx;
    unsigned long vsid;
    int index;

    /* Get pteg index */
    vsid = 0xffff0 | ((va & 0xf0000000) >> 28);
    ptegidx = (vsid ^ ((va & ADDR_PIDX) >> ADDR_PIDX_SHFT)) & 0x1ffff;

    /* Search primary hash */
    ptegTable = (PTEG *)systemMemoryEnd;
    for (pt = ptegTable[ptegidx].pte, index = 0; index < PTEG_NUM; index++, pt++)
    {
        if ((va == PteToVa(pt)) && (pa == PteToPa(pt))) 
        {
            return (pt);
        }
    }

    /* Search secondary hash */
    ptegidx ^= 0x1ffff;
    for (pt = ptegTable[ptegidx].pte, index = 0; index < PTEG_NUM; index++, pt++)
    {
        if ((va == PteToVa(pt)) && (pa == PteToPa(pt))) 
        {
            return (pt);
        }
    }
#endif
    return (0); 
}

/**
 * TFXDelayByBusyLoop - Delay busy loop for duration nano seconds. 
 *
 * DESCRIPTION
 *  This function delaies the execution using busy loop method for duration
 *  whose unit is nano second.
 *
 * PARAMETERS
 *  duration [IN] - time value during which the busy loop is executed.
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
TFXDelayByBusyLoop(TimeValue duration)
{
    BspMsDelay((UInt32)(duration/TEST_MSEC));
}

/**
 * TFXMemCompare - compare bytes in memory
 *
 * DESCRIPTION
 *  The TFXMemCompare() function compares the first n bytes(each interpreted as unsigned 
 *  char) of the object pointed to by s1 to the first n bytes of the object pointed to by s2.
 *  The sign of a non-zero returns is determined by the sign of the difference
 *  between the values of the first pair of bytes that differ in the objects 
 *  being compared.
 *
 * PARAMETERS
 *  s1     - [in] Points to a address of memory object
 *  s2     - [in] Points to a address of memory object
 *  length - [in] Specifies the maximum length of memory to compare
 *
 * TRACEABILITY
 *  N/A
 *
 * RETURNS
 *  The TFXMemCompare() function returns an integer greater than, equal to or less
 *  than 0, if the object pointed to by s1 is greater than, equal to or less
 *  than the object pointed to by s2 respectively.
 *
 * ERRORS
 *  N/A
 */
int
TFXMemCompare(const void *s1, const void *s2, Size length)
{
    unsigned char *p1;
    unsigned char *p2;

    if (length == 0)
        return (0);

    p1 = (unsigned char *)s1;
    p2 = (unsigned char *)s2;

    do {
        if (*p1++ != *p2++)
            return (*--p1 - *--p2);
    } while (--length != 0);

    return (0);
}
