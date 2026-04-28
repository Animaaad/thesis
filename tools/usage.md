# Rools – Usage

## Files
This folder contains 4 files:

- `isomorph.ipynb` - a converter from graph6 format to adjacency list format,
- `g6ToAdjList.cpp` - a SageMath script for removing isomorphic graphs,
- `negators.cpp` - a generator of negators that serve as building blocks for the snark constructions.
- `23_poles_generator.cpp` - a generator of proper (2,3)-poles that serve as building blocks for the snark constructions.


## Required auxiliary files
- `isomorph.ipynb` depends on a preinstalled sage and a precomputed file of snarks `snarks.ba`.

- `g6ToAdjList.cpp` depends on a file of graphs in g6 format `snarks.g6` and prints the output into `snarks.ba`.

- both `negators.cpp` and `23_poles_generator.cpp` depend on a file `cubic.ba`, which can be obtained by getting a set of cubic graphs from "House of graphs" in g6 formal and converting them to .ba format using the program `/tools/g6ToAdjList`.

The programs do not take these file as a command-line argument.  
It expects the files to be present in the same directory as the executable at runtime.

## Compilation

Standard c++ compilation ad execution for all the files, except `isomorph.ipynb`.