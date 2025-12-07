#!/bin/bash

THREADS_LIST="2 4 8 16 32"
RUNS=5

TIME_CMD="/usr/bin/time"
TIME_FORMAT="%e;%U;%S" # real;user;sys

MEASURE_DIR="measures"

echo "threads;run;real;user;sys" > "$MEASURE_DIR/philosophes.csv"

for n in $THREADS_LIST; do
    for run in $(seq 1 $RUNS); do
        res=$($TIME_CMD -f "$TIME_FORMAT" ./philosophes "$n" 2>&1 > /dev/null)
        echo "$n;$run;$res" >> "$MEASURE_DIR/philosophes.csv"
    done
done

echo "threads_prod;threads_cons;run;real;user;sys" > "$MEASURE_DIR/prodcons.csv"

for total in $THREADS_LIST; do
    prod=$((total / 2))
    cons=$((total / 2))
    for run in $(seq 1 $RUNS); do
        res=$($TIME_CMD -f "$TIME_FORMAT" ./prod-cons "$prod" "$cons" 2>&1 > /dev/null)
        echo "$prod;$cons;$run;$res" >> "$MEASURE_DIR/prodcons.csv"
    done
done

echo "ecrivains;lecteurs;run;real" > "$MEASURE_DIR/lecteurs_ecrivains.csv"

for total in $THREADS_LIST; do
    ecrivains=$((total / 2))
    lecteurs=$((total / 2))

    for run in $(seq 1 $RUNS); do
        start=$(date +%s%N)
        ./lecteurs-ecrivains "$ecrivains" "$lecteurs" > /dev/null
        end=$(date +%s%N)

        elapsed_ns=$((end - start))
        elapsed_s=$(echo "$elapsed_ns / 1000000000" | bc -l)

        echo "$ecrivains;$lecteurs;$run;$elapsed_s" >> "$MEASURE_DIR/lecteurs_ecrivains.csv"
    done
done

echo "threads;run;real" > "$MEASURE_DIR/test_tas.csv"

for n in $THREADS_LIST; do
    for run in $(seq 1 $RUNS); do
        res=$($TIME_CMD -f "%e" ./test-tas "$n" 2>&1 > /dev/null)
        echo "$n;$run;$res" >> "$MEASURE_DIR/test_tas.csv"
    done
done

