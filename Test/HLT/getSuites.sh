cat `find . -name "*alltests.csv"` | sort > results_1.csv
cat `find . -name "*suite.csv"`    | sort > results_2.csv
cat results_1.csv results_2.csv | sort > hlt_suite_results.csv
rm results_1.csv results_2.csv
