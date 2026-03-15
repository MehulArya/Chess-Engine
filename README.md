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

---

## Move Representation (`move.h`, `move.cpp`)

The engine represents chess moves using a compact **32-bit encoding** designed for efficient storage and fast access during search.  
Each move is stored inside a `Move` structure which internally packs all move information into a single 32-bit integer.

This design enables:

- fast copying of moves
- efficient storage in move lists
- quick extraction of move properties using bit operations
- minimal memory overhead during search

---

### Move Encoding Layout

The move representation stores several pieces of information inside a single integer using bit packing.

| Bits | Field | Description |
|-----|------|-------------|
| 0–5 | To Square | Destination square (0–63) |
| 6–11 | From Square | Origin square (0–63) |
| 12–15 | Promotion Piece | Piece type promoted to |
| 16–19 | Moving Piece | Piece that moves |
| 20–23 | Captured Piece | Captured piece (if any) |
| 24–27 | Move Flag | Special move type |

This layout allows the engine to reconstruct all move information using simple bit shifting and masking operations.

---

### Move Flags

Special move types are represented using the `MoveFlag` enumeration.  
These flags allow the engine to identify special moves quickly during move execution and search.

Move flags include:

- Quiet moves
- Captures
- Double pawn pushes
- King-side castling
- Queen-side castling
- En passant captures
- Pawn promotions
- Promotion captures

Using flags simplifies the move execution logic and allows the engine to distinguish between different move behaviors efficiently.

---

### Move Construction

Moves are created using a static factory function that packs all move information into the internal 32-bit representation.

This function receives:

- the origin square
- the destination square
- the moving piece
- the captured piece (if any)
- the promotion piece (if applicable)
- the move flag describing the move type

All values are packed into the move's internal integer using bit operations.

---

### Move Query Functions

The `Move` structure provides several helper methods that extract information from the encoded move.

These functions allow other engine modules to retrieve:

- origin square
- destination square
- moving piece
- captured piece
- promotion piece
- move type

Additional utility checks are provided to determine whether a move:

- captures a piece
- promotes a pawn

These helper methods make the move representation easy to use throughout the engine.

---

### UCI Move Conversion

The engine supports conversion between the internal move representation and the **UCI (Universal Chess Interface)** move format.

UCI represents moves as simple coordinate strings such as:

- e2e4
- g1f3
- e7e8q

Two utility functions are implemented:

- converting an internal move to a UCI string
- parsing a UCI string into move components

This functionality is essential for communicating with chess GUIs such as Arena, CuteChess, or ChessBase.

---

### Design Goals

The move system is designed with several important goals:

- compact representation
- fast bitwise operations
- minimal memory usage
- efficient storage in move lists
- compatibility with the UCI protocol

This move representation will be heavily used by core engine components including move generation, board updates, search algorithms, and GUI communication.

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


