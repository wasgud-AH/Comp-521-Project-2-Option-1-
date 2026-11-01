#!/usr/bin/env bash
set -euo pipefail

OUTPUT="${1:-sync_results.csv}"
PROGRAM="./sync_project2"
TOTAL_ITERATIONS=12000000
TRIALS=3

gcc -O2 -Wall -Wextra -pthread sync_project2.c -o sync_project2

echo "workers,total_iterations,use_mutex,trial,final_counter,elapsed_seconds" > "$OUTPUT"

for workers in 1 2 4; do
    for mode in 0 1; do
        for trial in $(seq 1 "$TRIALS"); do
            "$PROGRAM" "$workers" "$TOTAL_ITERATIONS" "$mode" "$trial" >> "$OUTPUT"
        done
    done
done

echo "Wrote $(($(wc -l < "$OUTPUT") - 1)) data rows to $OUTPUT"
