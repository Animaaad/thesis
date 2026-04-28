# Class even2_2_2 – Usage

## Files
This folder contains two implementations of the even2_2_2 class snark generator:

- `even2_2_2.cpp` – standard variant of the construction  
- `even2_2_2_flipped.cpp` – modified variant where connector pairings are flipped in all applicable cases

Both implementations realize the same underlying construction, but differ in how connections between multipole connectors are chosen.

## Required auxiliary files
Both programs depend on a file:

- `cubic.ba`

This file can be obtained by getting a set of cubic graphs from "House of graphs" in g6 formal and converting them to .ba format using the program `/tools/g6ToAdjList`

The program does not take this file as a command-line argument.  
It expects `cubic.ba` to be present in the same directory as the executable at runtime.

## Compilation

Compilation

Standard variant:

g++ even2_2_2.cpp -o even2_2_2

Flipped variant:

g++ even2_2_2_flipped.cpp -o even2_2_2_flipped

Execution

Standard variant:

./even2_2_2

Flipped variant:

./even2_2_2_flipped