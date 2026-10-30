#!/usr/bin/env bash
set -euo pipefail

OUTPUT="${1:-sync_results.csv}"
PROGRAM="./sync_project2"
TOTAL_ITERATIONS=12000000
TRIALS=3

# TODO:
# 1. Compile sync_project2.c into the executable named sync_project2.
# 2. Use:
#       gcc -O2 -Wall -Wextra -pthread
# 3. Write this header to "$OUTPUT":
#       workers,total_iterations,use_mutex,trial,final_counter,elapsed_seconds
# 4. Run worker counts 1, 2, and 4.
# 5. Run mutex modes 0 and 1.
# 6. Run TRIALS trials for every combination.
# 7. Append each result to "$OUTPUT".
#
# Mode meanings:
#   0 = no mutex
#   1 = mutex enabled
#
# There should be:
#   3 workers x 2 modes x 3 trials = 18 data rows
#
# The command format is:
#   ./sync_project2 WORKERS TOTAL_ITERATIONS USE_MUTEX TRIAL

echo "Complete the TODO sections before running this script."
exit 1
