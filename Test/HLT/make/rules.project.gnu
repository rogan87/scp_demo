################################################################################
#
# neos: rules.bsp.gnu - target bsp makefile for GNU tools 
#
# Copyright(c) 2008 MDS Technology Co.,Ltd. 
# All right reserved
# 
# This software contains confidential information of MDS Technology Co.,Ltd.
# and unauthorized distribution of this software, or ay portion of it, are
# prohibited.
#
# DESCRIPTION
# -----------
# Generic rules for target BSPs.
#
# Brief history
# -------------
# 2008.07.28, Dave: created.
#
################################################################################

# define the tools
sinclude $(NEOS_SRC)/make/rules.make.$(TOOLS)

# Object search paths
BSP_DIR = $(NEOS_SRC)/bsp/$(BSP)
USER_DIR = $(NEOS_SRC)/project/$(PROJECT)

## Kernel librarys
KERNEL_LIB   := --start-group -lkernel --end-group
BOOTROM_LIB  := --start-group -lkernel $(USER_LIBS) -lnmon -lcrc --end-group
LIBGCC       := `$(CC) $(CC_FLOAT) $(CC_ARCH) -print-libgcc-file-name`

## User libraries
LDFLAGS += -L$(USER_DIR)

## Target BSP
CC_INCLUDE  += -I$(NEOS_SRC)/BSP/$(BSP)

## User defined librarys
USER_LIBS    +=

## BSP object files
BSP_OBJS := $(BSP_DIR)/RamInit.o
BSP_OBJS += $(BSP_DIR)/BspAsmLib.o
BSP_OBJS += $(BSP_DIR)/BspL1Cache.o
BSP_OBJS += $(BSP_DIR)/BspTimer.o
BSP_OBJS += $(BSP_DIR)/DebugConsole.o

