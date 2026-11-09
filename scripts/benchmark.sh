
#!/bin/bash

CORPUS="data/benchmark/documents_100000.txt"
THREAD_COUNTS=(4 6 8 12)
RUNS=5

for threads in "${THREAD_COUNTS[@]}"; do
    total=0
    min=999999
    max=0

    echo "Testing $threads threads..."

    for ((run=1; run<=RUNS; run++)); do
        echo "Run $run"

        elapsed=$({ 
            /usr/bin/time -f "%e" \
            ./build/needle --build "$CORPUS" "$threads" \
            >/dev/null
        } 2>&1)

        echo "Time: $elapsed seconds"

        total=$(awk -v a="$total" -v b="$elapsed" \
            'BEGIN {print a+b}')

        min=$(awk -v a="$min" -v b="$elapsed" \
            'BEGIN {print (a < b) ? a : b}')

        max=$(awk -v a="$max" -v b="$elapsed" \
            'BEGIN {print (a > b) ? a : b}')
    done

    average=$(awk -v total="$total" -v runs="$RUNS" \
        'BEGIN {printf "%.3f", total/runs}')

    echo "Average: $average seconds"
    echo "Minimum: $min seconds"
    echo "Maximum: $max seconds"
    echo "-----------------------------"
done
