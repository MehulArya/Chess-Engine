# ♟️ Chess Engine in C++

##  Description

This project is a chess engine written in C++ that aims to implement classical artificial intelligence techniques such as Minimax search and Alpha–Beta pruning to compute optimal chess moves.
The engine is being built from scratch with a strong focus on understanding internal architecture, data representation, and performance-oriented design.

Currently, the project is in its **foundation stage**, where core data types and low-level representations used throughout the engine are being defined.

---

## Current Progress

The foundational type system has been implemented.

Completed:

* Core engine type definitions
* Square representation system
* Bitboard utility helpers
* Castling rights flags
* Inline helper functions for fast computation

The project is currently establishing the base abstractions required before implementing board logic and move generation.

---

## Type System (`types.h`)

The `types.h` module defines the fundamental data structures used across the entire engine.
This file acts as the backbone of the engine’s architecture and ensures that all future modules share consistent representations.

It currently includes:

**Square Representation**

* Squares are encoded as integers (0–63)
* Helper functions exist to extract file and rank

**Bitboards**

* Bitboards are used to represent board positions efficiently
* Each bit corresponds to one square
* Enables fast move generation using bitwise operations

**Castling Rights**

* Stored as bit flags for efficient combination and checking
* Allows quick validation of legal castling moves

**Inline Utility Functions**

* `make_sq()` → creates square index
* `file_of()` → extracts file
* `rank_of()` → extracts rank
* `bb_of()` → returns bitboard of a square

These utilities are designed for:

* speed
* compile-time evaluation
* zero runtime overhead

---

## Planned Architecture

The engine will be modular and organized into separate components:

| Module  | Responsibility                   |
| ------- | -------------------------------- |
| Board   | Stores game state                |
| Move    | Represents moves                 |
| MoveGen | Generates legal moves            |
| Search  | Finds best move using algorithms |
| Eval    | Evaluates positions              |
| UCI     | Communicates with chess GUIs     |

---

## Build Instructions

Clone and build using CMake:

```bash
git clone https://github.com/MehulArya/Chess-Engine.git
mkdir build && cd build
cmake ..
make
```

Executable will be available once core engine modules are implemented.

---

## Project Goal

This project is being built to deeply understand:

* game tree search algorithms
* engine architecture design
* low-level performance optimization
* bit manipulation techniques

The focus is not just building a chess engine, but understanding how engines are engineered internally.

---

## 👨‍💻 Author

**Mehul Arya**


