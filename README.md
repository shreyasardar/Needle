# Needle — C++ Search Engine

## Performance Benchmarks

Benchmarked index construction on a synthetic corpus of 100,000 documents.

### Index Build Time

| Threads | Run 1 (seconds) | Run 2 (seconds) | Average (seconds) |
|---:|---:|---:|---:|
| 1 | 4.384 | 4.419 | 4.402 |
| 2 | 2.590 | 2.621 | 2.606 |
| 4 | 1.689 | 1.768 | 1.729 |
| 8 | 1.432 | 1.460 | 1.446 |
| 12 | 1.254 | 1.373 | 1.314 |

### Results

- Best observed configuration: 12 threads.
- Average build time decreased from 4.402 seconds with 1 thread to 1.314 seconds with 12 threads.
- Observed speedup: approximately 3.35×.
- Elapsed-time reduction: approximately 70.1%.
- Index correctness was checked by comparing the sorted index files produced by 1-thread and 8-thread builds; they matched.

*Note: Results are from two benchmark runs on the same machine. Performance may vary with system load and hardware.*
