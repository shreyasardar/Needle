# Needle — C++ Search Engine

## Performance Benchmarks

Benchmarked index construction on a synthetic corpus of 100,000 documents.

### Index Build Time

| Threads | Run 1 (seconds) | Run 2 (seconds) | Run 3 (seconds) | Average (seconds) |
|---:|---:|---:|---:|---:|
| 1 | 4.87 | 4.69 | 4.50 | 4.687 |
| 2 | 3.03 | 2.92 | 2.97 | 2.973 |
| 4 | 2.11 | 2.10 | 2.03 | 2.080 |
| 8 | 1.67 | 1.76 | 1.78 | 1.737 |
| 12 | 1.59 | 1.51 | 1.50 | 1.533 |

### Results

- Best tested configuration: 12 threads.
- Average build time decreased from 4.687 seconds with 1 thread to 1.533 seconds with 12 threads.
- Observed speedup: approximately 3.06×.
- Elapsed-time reduction: approximately 67.3%.
- Each configuration was tested three times on a synthetic corpus of 100,000 documents.
- Index correctness was checked by comparing sorted index files from 1-thread and 8-thread builds; they matched.

*Note: Results were measured on the same machine. Performance may vary with system load and hardware.*
