################################################################################
# 
# NEOS: rules.suite.mk - makefile for high level test suites
#
# Copyright(c) 2008 MDS Technology Co.,Ltd. 
# All right reserved
#
# This software contains confidential information of MDS Technology Co.,Ltd.
# and unauthorized distribution of this software, or any portion of it, are
# prohibited.
# 
# Brief history
# -------------
# 2008.08.01, dave.cassidy: created.
#
################################################################################

## test suite rules

define TEST_SUITE_template

    # add to the list of suite names
    TEST_SUITES += $(1)

    # top level dependency
    $(1): $$(OBJ_DIR)/$(1).elf
		@echo $(1) done

    # variable definitions
    $$(OBJ_DIR)/$(1).elf: TEST_SUITE_OBJS = $(2)
    $$(OBJ_DIR)/$(1).elf: TEST_SUITE_ID   = $(3)

    # dependencies
    $$(OBJ_DIR)/$(1).elf:  $$(TFX_OBJS)
    $$(OBJ_DIR)/$(1).elf:  $(2)
    $$(OBJ_DIR)/$(1).elf:  $(3)
    $$(OBJ_DIR)/$(1)_ID.o: $$(MAKE_DIR)/$(1).mk
    $$(OBJ_DIR)/$(1)_ID.c: $$(MAKE_DIR)/$(1).mk

    # test suite build rule
    $$(OBJ_DIR)/$(1).elf: $$(CONFIG_FILE)
		@echo generating $$(@) $$(CONFIG_FILE)
		@$$(LD) $$(LDFLAGS) -T $$(KERN_LDSCRIPT) --section-start .kernel.text=$$(RAM_START_ADRS)\
         $$(BSP_OBJS) $$(CONFIG_FILE) $$(TFX_OBJS) \
		 $$(TEST_SUITE_OBJS) $$(TEST_SUITE_ID) \
		 $$(KERNEL_LIB) $$(LIBGCC) $$(USER_LIBS) -o $$(@)
		@$$(NM) $$(@) | sort > $$(@).map

    # suite ID build rule
    $$(OBJ_DIR)/$(1)_ID.c:
		@$$(ID_GEN) --suiteName=$(1) $(2)

endef
