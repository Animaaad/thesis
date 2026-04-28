# Edge_reduction – Usage

## Files
This folder contains implementation of a generator that produces new snarks by edge-reducing snarks that are one or two reductions away from being critical with the following file:

- `edge_reduction.cpp`  

## Required auxiliary files
The program depends on a file with a very specific format, not a typical .ba format, it has snarks, which tested by a specific program, and prints which are cocritical, which are critical and which edge reductions need to be performed to get critical/cocritical graphs on graphs that are not critical/cocritical. The file name is expected to be `snarks.ba`.

The program does not take this file as a command-line argument.  
It expects `snarks.ba` to be present in the same directory as the executable at runtime.

## Compilation

Compilation

g++ edge_reduction.cpp -o edge_reduction

Execution

./edge_reduction
