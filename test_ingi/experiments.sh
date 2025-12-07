#!/bin/bash

THREADS_LIST="2 4 8 16 32"
RUNS=5

echo "PHILOSOPHES"
echo "threads;run;real"

for n in $THREADS_LIST; do
    for run in $(seq 1 $RUNS); do
        start=$(date +%s%N)
        ./philosophes "$n" > /dev/null
        end=$(date +%s%N)

        elapsed_ns=$((end - start))
        # conversion nanosecondes -> secondes avec awk (pas besoin de bc)
        elapsed_s=$(awk -v ns="$elapsed_ns" 'BEGIN { printf "%.9f", ns/1000000000 }')

        echo "$n;$run;$elapsed_s"
    done
done

echo
echo "PROD-CONS"
echo "threads_prod;threads_cons;run;real"

for total in $THREADS_LIST; do
    prod=$((total / 2))
    cons=$((total / 2))

    for run in $(seq 1 $RUNS); do
        start=$(date +%s%N)
        ./prod-cons "$prod" "$cons" > /dev/null
        end=$(date +%s%N)

        elapsed_ns=$((end - start))
        elapsed_s=$(awk -v ns="$elapsed_ns" 'BEGIN { printf "%.9f", ns/1000000000 }')

        echo "$prod;$cons;$run;$elapsed_s"
    done
done

echo
echo "LECTEURS-ECRIVAINS"
echo "ecrivains;lecteurs;run;real"

for total in $THREADS_LIST; do
    ecrivains=$((total / 2))
    lecteurs=$((total / 2))

    for run in $(seq 1 $RUNS); do
        start=$(date +%s%N)
        ./lecteurs-ecrivains "$ecrivains" "$lecteurs" > /dev/null
        end=$(date +%s%N)

        elapsed_ns=$((end - start))
        elapsed_s=$(awk -v ns="$elapsed_ns" 'BEGIN { printf "%.9f", ns/1000000000 }')

        echo "$ecrivains;$lecteurs;$run;$elapsed_s"
    done
done

