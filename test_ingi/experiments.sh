#!/bin/bash

THREADS_LIST="1 2 4 8 16 32"
RUNS=5

echo "===== TEST LOCK TAS ========"
echo "threads;run;real"

for n in $THREADS_LIST; do
    for run in $(seq 1 $RUNS); do

        # mesure avec /usr/bin/time car bc n’existe pas sur INGInious
        real=$(/usr/bin/time -f "%e" ./test-tas "$n" 2>&1 > /dev/null)

        echo "$n;$run;$real"
    done
done

