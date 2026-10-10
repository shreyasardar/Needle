#!/bin/bash

CORPUS="data/benchmark/documents_100000.txt"
THREAD_COUNTS=(1 2 4 8 12)
RUNS=3

for threads in "${THREAD_COUNTS[@]}"; do
    total=0

    echo "Testing $threads threads..."

    for ((run=1; run<=RUNS; run++)); do
        echo "Run $run"

       elapsed=$({ /usr/bin/time -f "%e" \
    ./build/needle --build "$CORPUS" "$threads" \
    >/dev/null; } 2>&1)
        echo "Time: $elapsed seconds"

        total=$(awk -v a="$total" -v b="$elapsed" \
            'BEGIN {print a+b}')
    done

    average=$(awk -v total="$total" -v runs="$RUNS" \
        'BEGIN {printf "%.3f", total/runs}')

    echo "Average for $threads threads: $average seconds"
    echo "-----------------------------"
done
