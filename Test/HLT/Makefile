################################################################################
# 
# NEOS: Makefile - HLT makefile 
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

# --------------
# Configuration
# --------------
#TESTTYPE := suites
TESTTYPE := individual

# -----------------
# local directories
# -----------------
VCOVER_DIR  := ./VCAST
TOOLS_DIR   := ./Tools

#SUBSYS := Cache
SUBSYS := Communication
SUBSYS += ExceptionHandling
SUBSYS += HealthMonitor
SUBSYS += InterruptHandling
SUBSYS += MemoryAllocation
SUBSYS += MemoryConfiguration
SUBSYS += Synchronization
SUBSYS += SystemIdentification
SUBSYS += SystemState
SUBSYS += ThreadManagement
SUBSYS += TimeManagement

.PHONY: $(SUBSYS)

#---------------
# Public targets
#---------------
#all: init run_tests register_coverage report
all: clean_tests build_individual run_individual report_test_results

init: create_cov_env build_$(TESTTYPE)

run_tests: run_$(TESTTYPE)

register_coverage: register_$(TESTTYPE)_coverage

report: report_coverage report_test_results

clean: clean_tests clean_coverage

help:
	@echo -- COMMAND LINE TARGETS --
	@echo "all                 run all the tests and make all tests report"
	@echo "init                build VCAST coverage environment and LLTPs"
	@echo "clean               clean all the tests and result files"
	@echo "run_tests           run all the tests"
	@echo "register_coverage   register coverage data to VectorCAST/Cover"
	@echo "report              compose test result and coverage files"
	@echo "help                generate this list"

#----------------
# Private targets
#----------------
create_cov_env:
	@echo HLT coverage environment is being created
	@$(MAKE) -C $(VCOVER_DIR) -f VCAST.mk environment > /dev/null
	@echo HLT coverage environment is created

compile:
	@$(foreach dir,$(SUBSYS),echo compiling in $(dir);make -s -C $(dir) NEOS178S_BASE=$(NEOS178S_BASE); echo;)
	@echo "compile DONE"

run_suites:
	@$(foreach dir,$(SUBSYS),echo running suites in $(dir); \
	make -s -C $(dir) run_suites; echo;)
	@echo "run_suites DONE"

run_individual:
	@$(foreach dir,$(SUBSYS),echo running tests in $(dir); \
	make -s -C $(dir) run_tests; echo;)
	@echo "run_tests DONE"

build_individual:
	@$(foreach dir,$(SUBSYS),echo building tests in $(dir); \
	make -s -C $(dir) tests; echo;)
	@echo "tests DONE"

build_suites:
	@$(foreach dir,$(SUBSYS),echo building suites in $(dir); \
	make -s -C $(dir) suites; echo;)
	@echo "suites DONE"

register_suites_coverage:
	@$(foreach dir,$(SUBSYS),echo posting coverage in $(dir); \
	make -s -C $(dir) suites_coverage; echo;)
	@echo "suites_coverage DONE"

register_individual_coverage:
	@$(foreach dir,$(SUBSYS),echo posting coverage in $(dir); \
	make -s -C $(dir) coverage; echo;)
	@echo "coverage DONE"

report_coverage:
	@$(MAKE) -s -C $(VCOVER_DIR) -f VCAST.mk report
	@echo "report DONE"

report_test_results:
	@sh $(NEOS178S_TEST_BASE)/HLT/getTests.sh

clean_tests:
	@$(foreach dir,$(SUBSYS),echo cleaning $(dir); \
	make -s -C $(dir) clean; echo;)
	@echo "clean DONE"

clean_coverage:
	@$(MAKE) -C $(VCOVER_DIR) -f VCAST.mk clean

uninst:
	@$(MAKE) -C $(VCOVER_DIR) -f VCAST.mk uninst

$(SUBSYS):
	@echo making $@
	@make -s -C $@
	@echo

