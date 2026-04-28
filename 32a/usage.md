# Class 32-A – Usage

## Files
This folder contains two implementations of the 32-A class snark generator:

- `32A.cpp` – standard variant of the construction  
- `32A_flipped.cpp` – modified variant where connector pairings are flipped in all applicable cases

Both implementations realize the same underlying construction, but differ in how connections between multipole connectors are chosen.

## Required auxiliary files
Both programs depend on a precomputed file:

- `negators.ba`

This file is generated using:
- `tools/negator_generator.cpp`

The program does not take this file as a command-line argument.  
It expects `negators.ba` to be present in the same directory as the executable at runtime.

## Compilation

Compilation

Standard variant:

g++ 32A.cpp -o 32A

Flipped variant:

g++ 32A_flipped.cpp -o 32A_flipped

Execution

Standard variant:

./32A

Flipped variant:

./32A_flipped