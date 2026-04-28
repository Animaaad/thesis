# Snark Generation Project

## Overview
This project implements generators for six specific classes of snarks, each with two variants.
In addition, it includes a generator based on edge reduction.

All classes, as well as the edge-reduction method used to generate new snarks, are described in Chapter 2 of the thesis *"Generating Infinite Families of Snarks"* by Maksym Maltsev.

## Structure
Each folder corresponds to one class (except for the `edge_reduction` folder).

Each class contains two implementations representing slightly different constructions:
- one generates snarks using a fixed way of connecting the connectors between multipoles,
- the other considers (in most cases) all possible ways of flipping these connections.

The `edge_reduction` folder contains an implementation of a generator that produces new snarks by edge-reducing snarks that are one or two reductions away from being critical.

The `tools` folder contains auxiliary scripts used in the workflow:
- a converter from graph6 format to adjacency list format,
- a SageMath script for removing isomorphic graphs,
- two additional generators of multipoles that serve as building blocks for the snark constructions.

## Data
The generated snarks, along with additional files required to run the project, are available here:  
https://drive.google.com/drive/folders/12kplx_bxTRjb6bBeCyU86kh2fLLoXtXp

## Usage
Each folder contains a `usage.md` file explaining how to compile and run the corresponding programs.

Isomorphism filtering requires SageMath. Since installing SageMath can be time-consuming, precomputed data is provided in the link above.