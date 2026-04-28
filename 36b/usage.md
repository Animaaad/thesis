# Class 36-B – Usage

## Files
This folder contains two implementations of the 36-B class snark generator:

- `36b.cpp` – standard variant of the construction  
- `36b_flipped.cpp` – modified variant where connector pairings are flipped in all applicable cases

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

g++ 36b.cpp -o 36b

Flipped variant:

g++ 36b_flipped.cpp -o 36b_flipped

Execution

Standard variant:

./36b

Flipped variant:

./36b_flipped