# Class 34-C – Usage

## Files
This folder contains two implementations of the 34-C class snark generator:

- `34c.cpp` – standard variant of the construction  
- `34c_flipped.cpp` – modified variant where connector pairings are flipped in all applicable cases

Both implementations realize the same underlying construction, but differ in how connections between multipole connectors are chosen.

## Required auxiliary files
Both programs depend on precomputed files:

- `negators.ba`
- `23poles.ba`

This file are generated using, respectively:
- `tools/negator_generator.cpp`
- `tools/23_pole_generator.cpp`

The program does not take this file as a command-line argument.  
It expects `negators.ba` and `23poles.ba` to be present in the same directory as the executable at runtime.

## Compilation

Compilation

Standard variant:

g++ 34c.cpp -o 34c

Flipped variant:

g++ 34c_flipped.cpp -o 34c_flipped

Execution

Standard variant:

./34c

Flipped variant:

./34c_flipped