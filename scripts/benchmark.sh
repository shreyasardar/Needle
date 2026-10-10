#!/bin/bash

CORPUS="data/benchmark/documents_100000.txt"
THREAD_COUNTS=(1 2 4 8 12)

for threads in "${THREAD_COUNTS[@]}"; do
    echo "Testing $threads threads..."

    time ./build/needle --build "$CORPUS" "$threads"
done