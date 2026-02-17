# BranchBench – ROOT I/O & Compute Benchmark

A performance study comparing:

- Manual TTree event loop
- ROOT RDataFrame execution

## Motivation

High Energy Physics workflows depend heavily on ROOT-based I/O.
This project studies:

- Compute-bound scaling
- Framework abstraction overhead
- Manual vs RDataFrame performance behavior

## Workload

- 1 scalar branch
- 1 vector<float> branch (size 20)
- Gaussian-distributed values
- Heavy per-element trigonometric compute
- Deterministic random seed (42)

## Build

mkdir build
cd build
cmake ..
make

## Run

./io_benchmark 500000
./io_benchmark 5000000
./io_benchmark 20000000

## System Used for Benchmark

- Ubuntu 24.04 (WSL)
- g++ 13.3
- ROOT 6.x
- Single-thread execution

## Scaling Results (Heavy Compute)

| Events     | Manual (ev/s) | RDataFrame (ev/s) |
|------------|---------------|-------------------|
| 500,000    | 98k           | 76k               |
| 5,000,000  | 102k          | 95k               |
| 20,000,000 | 100k          | 96k               |

## Observations

- Throughput stabilizes at ~100k events/s across large datasets.
- This indicates compute-bound behavior (linear time scaling with N).
- For small event counts (e.g. 500k), RDataFrame shows noticeable startup overhead.
- As dataset size increases, this fixed setup cost is amortized.
- For large datasets (20M events), RDataFrame overhead reduces to ~4–5%.
- In the steady-state regime, abstraction overhead becomes minor compared to per-event compute cost.
Note: Relative differences are larger at small event counts due to framework initialization costs that are amortized at scale.
