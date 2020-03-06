################################################################################
# 
# NEOS: rules.HLT.mk - makefile for subsystem high level tests
#
# Copyright(c) 2009 MDS Technology Co.,Ltd. 
# All right reserved
#
# This software contains confidential information of MDS Technology Co.,Ltd.
# and unauthorized distribution of this software, or any portion of it, are
# prohibited.
# 
# Brief history
# -------------
# 2008.07.29, dave.cassidy: created.
#
################################################################################


## Target processor information
ARCH      	= powerpc
CPU         = ppce200z7
CPU_VARIANT = mpc5674
BSP         = mpc567x-evb
PROJECT     = myproj
TOOLS       = gnu

RAM_START_ADRS  = 0x40002000
ROM_START_ADRS  = 0x40002000
XIP_START_ADRS  = 0x00000000
BOOTROM_ADRS    = 0xFFD00000
BOOTROM_SIZE    = 0x00300000

## Export variables
export ARCH CPU CPU_VARIANT TOOLS BSP PROJECT
export RAM_START_ADRS BOOTROM_ADRS BOOTROM_SIZE

# ------------------
#  Linker script
#  -----------------
KERN_LDSCRIPT := $(NEOS178S_BASE)/make/ldscripts/link.$(ARCH).ram
KERN_ROM_LDSCRIPT := --defsym=bootrom_adrs=$(BOOTROM_ADRS) \
                    -T $(NEOS178S_BASE)/make/ldscripts/link.$(ARCH).rom
## Linker script for xip
KERN_XIP_LDSCRIPT := $(NEOS178S_BASE)/make/ldscripts/link.$(ARCH).xip

# ------------------
# global directories
# ------------------
GLOBAL_TOOLS_DIR := $(NEOS178S_TEST_BASE)/Tools
NEOS_SRC         := $(NEOS178S_BASE)

# -----------------
# local directories
# -----------------
RULES_DIR       := ../make
TOOLS_DIR       := ../Tools
TFX_DIR         := ../TFX
SRC_DIR         := ./Source
OBJ_DIR         := ./Object
RESULT_DIR      := ./Result
MAKE_DIR        := ./make
TEMP_PWD        := $(shell cmd /C cd)
PWD             := $(subst \,/,$(TEMP_PWD))
CONFIG_DIR      := ./Config

TEMP_DIRS    := $(OBJ_DIR)
TEMP_DIRS    += $(RESULT_DIR)

# ----------------
# tool definitions
# ----------------
ID_GEN         := $(TOOLS_DIR)/genTestIds.pl
TEST_RUN       := $(GLOBAL_TOOLS_DIR)/neosExecutive.pl

# Tool options
log_tests := 1 

# -------------------------------
# header file include directories
# -------------------------------
SUBSYS_INCDIR   := ./Include
TEST_INCDIR     := ../Include
SYS_INCDIR      := $(NEOS_SRC)/include
BSP_INCDIR      := $(NEOS_SRC)/bsp/$(BSP)

INCLUDES += -I$(SUBSYS_INCDIR)
INCLUDES += -I$(SYS_INCDIR)
INCLUDES += -I$(BSP_INCDIR)
INCLUDES += -I$(TEST_INCDIR)
INCLUDES += -I$(NEOS_SRC)

# -----------------
# compiler switches
# -----------------

TESTCFLAGS += $(CFLAGS) -DPOWERPC32

# ------------
# source files
# ------------
SRC_FILES    := $(wildcard $(SRC_DIR)/*.c)
TEMP_OBJS    := $(SRC_FILES:%.c=%.o)
OBJECTS      := $(subst $(SRC_DIR),$(OBJ_DIR),$(TEMP_OBJS))
TESTS        := $(OBJECTS:%.o=%.elf)
TESTS_BIN    := $(OBJECTS:%.o=%.bin)
DEPENDENCIES := $(OBJECTS:%.o=%.mk)
TEST_IDS     := $(OBJECTS:%.o=%_ID.o)

# test execution results
TEST_RESULTS_TMP   := $(TESTS:%.elf=%.csv)
TEST_RESULTS       := $(subst $(OBJ_DIR),$(RESULT_DIR),$(TEST_RESULTS_TMP))
COVERAGE_FILES     := $(TEST_RESULTS:%.csv=%)
COVERAGE_FILES     := $(subst $(RESULT_DIR),$(COVERAGE_DIR),$(COVERAGE_FILES))

# suite execution results
SUITE_MAKE_FILES     := $(wildcard $(MAKE_DIR)/*.mk)
SUITE_RESULTS_FILES  := $(SUITE_MAKE_FILES:%.mk=%.csv)
SUITE_RESULTS_FILES  := $(subst $(MAKE_DIR),$(RESULT_DIR),$(SUITE_RESULTS_FILES))
SUITE_COVERAGE_FILES := $(SUITE_RESULTS_FILES:%.csv=%)
SUITE_COVERAGE_FILES := $(subst $(RESULT_DIR),$(COVERAGE_DIR),$(SUITE_COVERAGE_FILES))

# -----------
# TFX objects
# -----------
TFX_SRC      := $(wildcard $(TFX_DIR)/*.c)
TFX_ASMSRC   := $(wildcard $(TFX_DIR)/*.S)
TFX_TMP_OBJS := $(TFX_SRC:%.c=%.o) $(TFX_ASMSRC:%.S=%.o)
TFX_OBJS     := $(subst $(TFX_DIR),$(OBJ_DIR),$(TFX_TMP_OBJS))

# --------------------
# Configuration object
# --------------------
CONFIG_FILE := $(OBJ_DIR)/ConfigData.o $(OBJ_DIR)/TFXHelperBsp.o
.PRECIOUS: $(CONFIG_FILE)

# default rule: build the object files
# build everything
all: dirs depends tests coverage
	@echo  "All objects are UP TO DATE"

compile: dirs $(OBJECTS)

# include the dependency rules
-include $(wildcard $(OBJ_DIR)/*.mk) dummy.mk

# build the test images
tests: dirs depends neos $(TFX_OBJS) $(TEST_IDS) $(TESTS) $(TESTS_BIN)
	@echo  "All tests are UP TO DATE"

# build the neos (FIXME: neos should be built using make version 3.81)
neos:
	@$(MAKE) -s -C $(NEOS_SRC) &> /dev/null
	@$(MAKE) -s -C $(NEOS_SRC)/project/$(PROJECT) $(BSP_OBJS) > /dev/null
	@$(MAKE) -s -C $(NEOS_SRC)/project/$(PROJECT) $(BSP_DIR)/RamInit.o
	@cp $(BSP_DIR)/RamInit.o $(USER_DIR)
	@echo neos is up to date

# include the rules for test suites
-include $(RULES_DIR)/rules.suite.mk
-include $(wildcard $(MAKE_DIR)/*.mk) dummy.mk

# generate the test suite objects
TEST_SUITE_RESULTS := $(TEST_SUITES:%=$(RESULT_DIR)/%.csv)

# build the test suites
suites: neos $(OBJ_DIR) $(TEST_SUITES)
	@echo  "All test suites are UP TO DATE"

# test execution targets
run: dirs run_tests run_suites
run_tests: dirs $(TEST_RESULTS)
run_suites: dirs $(TEST_SUITE_RESULTS)

report:
	@$(MAKE) -C $(VCAST_DIR) -f VCAST.mk report

suites_coverage: $(SUITE_COVERAGE_FILES)  
	@echo coverage results posted

coverage: $(COVERAGE_FILES)  
	@echo coverage results posted

$(COVERAGE_DIR)/%: $(RESULT_DIR)/%.dat
	@echo posting $(@F)
	$(MAKE) -s -C $(VCAST_DIR) -f VCAST.mk cov_file=$(PWD)/$(<) cov_name=$(@F) coverage

# convert elf to bin
$(OBJ_DIR)/%.bin: $(OBJ_DIR)/%.elf
	@echo Converting $(<F)
	$(OBJCOPY) -O binary -R .note -R .comment -S $< $@

# test coverage rule
$(RESULT_DIR)/%.dat: $(OBJ_DIR)/%.elf
	@echo Executing $(<F)
	@$(TEST_RUN) --log=$(log_tests) $<

# test execution rule
$(RESULT_DIR)/%.csv: $(OBJ_DIR)/%.elf
	@echo Executing $(<F)
	@$(TEST_RUN) --log=$(log_tests) $< 
	@echo Executing is done.

# generate the TP_*_ID.c file
$(OBJ_DIR)/%_ID.c :
	@echo generating $(@F);
	@$(ID_GEN) --idName $(@);

# make a generated object file
$(OBJ_DIR)/%.o : $(OBJ_DIR)/%.c
	@echo $(CC) $(<F) -o $(@F);
	@$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@;

# make a test object file
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@echo $(CC) $(<F) -o $(@F);
	$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@;

# Build TFX objects
$(OBJ_DIR)/%.o: $(TFX_DIR)/%.c
	@echo $(CC) $< -o $@
	$(CC) $(TESTCFLAGS) $(INCLUDES) -c $< -o $@

# Build custom configuration files
$(OBJ_DIR)/%.o: $(TFX_DIR)/%.config
	@echo making CONFIGURATION: $@  from $<
	@$(CC) $(TESTCFLAGS) $(INCLUDES) -c -x c $< -o $@

# Build custom configuration files
$(OBJ_DIR)/%.o: $(CONFIG_DIR)/%.config
	@echo making CONFIGURATION: $@  from $<
	@$(CC) $(TESTCFLAGS) $(INCLUDES) -c -x c $< -o $@

$(OBJ_DIR)/%.o : $(TFX_DIR)/%.S
	@echo $(CC) $< -o $@
	@$(CC) $(AFLAGS) -c -o $@ $<

# Build kernel RAM image for tests
$(OBJ_DIR)/%.elf : TEST_ID = $(@:%.elf=%_ID.o)
$(OBJ_DIR)/%.elf : $(OBJ_DIR)/%.o $(CONFIG_FILE)
	@echo linking $@ $(CONFIG_FILE)
	@$(LD) $(LDFLAGS) -T $(KERN_LDSCRIPT) --section-start .kernel.text=$(RAM_START_ADRS) $(BSP_OBJS) $(CONFIG_FILE) \
    $(TFX_OBJS) $(TEST_ID) $< $(USER_LIBS) $(KERNEL_LIB) $(LIBGCC) -o $@
	@$(NM) $@ | sort > $@.map

# generate the dependencies
$(OBJ_DIR)/%.mk: $(SRC_DIR)/%.c
	@echo dependencies for $(@:.mk=.o)
	@$(CC) $(CC_DEFINES) -MM -MF $@ -MT $(@:.mk=.o) $(INCLUDES) $<

depends: $(DEPENDENCIES)
	@echo dependencies generated

# create the output directories
$(TEMP_DIRS):
	mkdir $@

dirs: $(TEMP_DIRS)

# clean up temporary files
clean:
	@rm -f $(SRC_DIR)/*.bak
	@rm -f $(OBJ_DIR)/*.mk
	@rm -f $(OBJ_DIR)/*.c
	@rm -f $(OBJ_DIR)/*.o
	@rm -f $(OBJ_DIR)/*.elf
	@rm -f $(OBJ_DIR)/*.map
	@rm -f $(OBJ_DIR)/*.bin
	@rm -f $(RESULT_DIR)/*.csv
	@rm -f $(RESULT_DIR)/*.dat
	@rm -f $(RESULT_DIR)/*.log
	@rm -r $(TEMP_DIRS)

cov_clean:
	@$(MAKE) -C $(VCAST_DIR) -f VCAST.mk clean

help:
	@echo -- COMMAND LINE VARIABLES --
	@echo "	log_tests=0"
	@echo -- COMMAND LINE TARGETS --
	@echo "	all             (default rule) build all tests and suites"
	@echo "	compile         compile all test source files"
	@echo "	tests           build all test images (TP_SRD_*.elf)"
	@echo "	suites          build all test suite images (*.elf)"
	@echo "	run             execute all tests and test suites"
	@echo "	run_tests       execute all tests"
	@echo "	neos            build the kernel"
	@echo "	run_suites      execute all test suites"
	@echo "	suites_coverage post the coverage results from suites to VectorCast"
	@echo "	depends         generate the dependency make files for the tests"
	@echo "	dirs            create $(TEMP_DIRS)"
	@echo "	clean           remove all generated objects"
	@echo "	cov_clean       remove all coverage information"
	@echo "	coverage        post the coverage results to VectorCast"
	@echo "	report          generate the VectorCast summary coverage report"
	@echo "	help            print this message"


# include the build rules
include $(RULES_DIR)/rules.project.$(TOOLS)
