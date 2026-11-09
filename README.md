# Needle — High-Performance C++ Search Engine

Needle is a C++ full-text search engine designed for efficient document indexing and relevance-based retrieval. It uses an inverted index, BM25 ranking, Boolean query processing, positional indexing, and multithreaded index construction.

## Features

- **Inverted Indexing:** Efficient mapping of terms to document postings.
- **BM25 Ranking:** Relevance-based document retrieval.
- **Boolean Search:** Supports AND, OR, and implicit AND operators.
- **Phrase Search:** Supports exact phrase queries using positional indexes.
- **Query Parsing:** Recursive-descent parser with parentheses and operator precedence.
- **Top-K Retrieval:** Returns the most relevant search results.
- **Persistent Indexing:** Saves and loads indexes with validation and error handling.
- **Multithreaded Indexing:** Parallel document indexing using multiple threads.
- **Input Validation:** Handles duplicate document IDs, invalid inputs, and malformed index files.
- **Automated Testing:** 50 test cases covering indexing, retrieval, query processing, and persistence.

## Technologies Used

- **Language:** C++
- **Build System:** CMake
- **Data Structures:** Hash Maps, Vectors, Inverted Index, Positional Postings
- **Ranking Algorithm:** BM25
- **Concurrency:** C++ Multithreading
- **Testing:** Custom automated test suite
- **Profiling:** Valgrind (Memcheck and Massif)

---

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
- Observed speedup: approximately **3.06×**.
- Elapsed-time reduction: approximately **67.3%**.
- Each configuration was tested three times on a synthetic corpus of 100,000 documents.
- Index correctness was checked by comparing sorted index files from 1-thread and 8-thread builds; they matched.

*Note: Results were measured on the same machine. Performance may vary with system load and hardware.*

---

## Query Performance Optimization

Optimized phrase search by reducing repeated posting-list scans.

### Optimizations Implemented

- Cached posting lookups using unordered maps.
- Precomputed IDF values for phrase terms.
- Used binary search to verify consecutive term positions.
- Reused cached posting information during BM25 score calculation.

### Query Benchmark Results

Measurements collected using 10 runs per query.

| Query | Before (ms) | After (ms) | Improvement |
|---|---:|---:|---:|
| `"machine learning"` | 149.711 | 8.769 | 94.14% |
| `"machine learning" OR database` | 155.221 | 14.510 | 90.65% |

Phrase search achieved approximately **17.1×** and **10.7×** faster execution, respectively.

*Note: Results were measured on the same machine. Performance may vary depending on hardware and system load.*

---

## Scalability Benchmarks

Evaluated index construction performance using synthetic datasets of different sizes.

### Dataset Scaling (8 Threads)

Each measurement represents the average of multiple runs.

| Documents | Average Indexing Time (seconds) | Throughput (docs/second) |
|---:|---:|---:|
| 100,000 | 1.694 | 59,018 |
| 250,000 | 4.111 | 60,819 |
| 500,000 | 8.239 | 60,683 |

### Results

- Indexed up to **500,000 documents**.
- Maintained an average throughput of approximately **60K documents per second**.
- Indexing time scales approximately linearly as dataset size increases.
- Benchmarks include document loading, tokenization, indexing, merging, and index saving.

## Thread Scaling Benchmark (500K Documents