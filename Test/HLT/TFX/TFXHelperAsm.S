/*
 * TFXHelperAsm.S - Test Helper Assembly Functions
 *
 * Copyright(c) 2008 MDS Technology Co.,Ltd.
 * All rights reserved.
 *
 * This software contains confidential information of MDS Technology Co.,Ltd.
 * and unauthorized distribution of this software, or any portion of it, are
 * prohibited.
 */
#include <asp/powerpc/ppce200z7/AssemblyLib.h>
#include <asp/powerpc/ppce200z7/Powerpc.h>

    .global TFXGenAlignExc
    .global TFXGenVmxAsstExc
    .global TFXGetDecrementer

/**
 * TFXGenAlignExc - Raise unaligned access exception
 *
 * DESCRIPTION
 *  This function raises unaligned access exception with value unalined.
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
TFXGenAlignExc:
    stmw 30, 0(r3) 
    blr

#if 0
/**
 * TFXGenVmxAsst - Raise VPU Assist exception
 *
 * DESCRIPTION
 *  This function raises VPU Assist exception with value pointed to by the first
 *  parameter.
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
TFXGenVmxAsstExc:
    li      r0, 0
    vxor    vr0, vr1, vr1
    mtvscr  vr0
    lvx     vr4, r0, r3
    vrefp   vr3, vr4
    blr
#endif

/**
 * TFXGetDecrementer - Read Decrementer register
 *
 * DESCRIPTION
 *  This function read decrementer register and returns its value.
 *
 * PARAMETERS
 *  N/A
 *
 * TRACEABILITY
 *  N/A
 *
 * RETURNS
 *  Decrementer register value
 *
 * ERRORS
 *  N/A
 */
TFXGetDecrementer:
    mfdec   r3
    blr

GLOBAL_FUNC(PpcSetFpscr)
    mtspr fr0, r3
    isync
    blr

GLOBAL_FUNC(PpcGetFpscr)
    mfspr r3, fr0
    isync
    blr
