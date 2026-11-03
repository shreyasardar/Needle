
#!/bin/bash

QUERY_FILE="data/benchmark/queries.txt"
RUNS=10
NEEDLE="./build/needle"

if [[ ! -f "$QUERY_FILE" ]]; then
    echo "Query file not found: $QUERY_FILE"
    exit 1
fi

if [[ ! -f "$NEEDLE" ]]; then
    echo "Needle executable not found. Build the project first."
    exit 1
fi

echo "======================================"
echo "       NEEDLE QUERY BENCHMARK"
echo "======================================"

while IFS= read -r query || [[ -n "$query" ]]; do

    # Skip empty lines
    [[ -z "${query//[[:space:]]/}" ]] && continue

    echo ""
    echo "Query: $query"

    total=0
    minimum=""
    maximum=""
    result_count=""

    for ((run=1; run<=RUNS; run++)); do

        output=$("$NEEDLE" --search "$query" 2>&1)

        elapsed=$(echo "$output" |
            awk '/Search time:/ {print $3; exit}')

        if [[ -z "$elapsed" ]]; then
            echo "Search failed:"
            echo "$output"
            exit 1
        fi

        count=$(echo "$output" |
            grep -c "Document ID:")

        total=$(awk -v a="$total" -v b="$elapsed" \
            'BEGIN {print a+b}')

        if [[ -z "$minimum" ]] || \
           awk -v a="$elapsed" -v b="$minimum" \
           'BEGIN {exit !(a < b)}'; then
            minimum=$elapsed
        fi

        if [[ -z "$maximum" ]] || \
           awk -v a="$elapsed" -v b="$maximum" \
           'BEGIN {exit !(a > b)}'; then
            maximum=$elapsed
        fi

        result_count=$count

        echo "Run $run: $elapsed ms"

    done

    average=$(awk -v total="$total" -v runs="$RUNS" \
        'BEGIN {printf "%.3f", total/runs}')

    echo "Results returned: $result_count"
    echo "Average: $average ms"
    echo "Minimum: $minimum ms"
    echo "Maximum: $maximum ms"
    echo "--------------------------------------"

done < "$QUERY_FILE"

echo ""
echo "Benchmark completed!"

