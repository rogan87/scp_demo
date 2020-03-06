# Generate suite results.
cat `find $NEOS178S_TEST_BASE/HLT -name "*alltests.csv"` | sort > results_1.csv
cat `find $NEOS178S_TEST_BASE/HLT -name "*suite.csv"`    | sort > results_2.csv
cat results_1.csv results_2.csv | sort > ../hlt_suite_results.csv
rm results_1.csv results_2.csv

