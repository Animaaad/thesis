# Class panchrom – Usage

## Files
This folder contains two implementations of the panchrom class snark generator:

- `panchrom.cpp` – standard variant of the construction  
- `even2_2_2_flipped.cpp` – modified variant where connector pairings are flipped in all applicable cases

Both implementations realize the same underlying construction, but differ in how connections between multipole connectors are chosen.

## Required auxiliary files
Both programs depend on a precomputed file:

- `4regular.ba`

This file can be found in this folder (`/4regular.ba`)

The program does not take this file as a command-line argument.  
It expects `4regular.ba` to be present in the same directory as the executable at runtime.

## Compilation

Compilation

Standard variant:

g++ panchrom.cpp -o panchrom

Flipped variant:

g++ panchrom_flipped.cpp -o panchrom_flipped

Execution

Standard variant:

./panchrom

Flipped variant:

./panchrom_flipped