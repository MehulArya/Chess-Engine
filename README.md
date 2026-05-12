# Chess Engine in C++

A work-in-progress chess engine written in modern C++.

The project is being built from scratch to explore engine architecture, compact move representation, board state management, attack generation, hashing, testing, and eventually full move generation and search.

## Current Status

The engine has moved beyond the initial type-system stage. It now contains the core board and move infrastructure needed before legal move generation and search can be completed.

Implemented so far:

- Core chess types in `types.h`
- Compact 32-bit move representation
- UCI coordinate move conversion and parsing
- Board state storage using a 64-square piece array
- FEN loading and serialization
- Castling rights, en passant square, halfmove clock, and fullmove number tracking
- Make/unmake move support with undo state
- Basic check and square-attack detection
- Precomputed knight, king, and pawn attack tables
- Sliding attacks for bishops, rooks, and queens
- Zobrist key generation for board positions
- CMake build setup with warning configuration and optional sanitizers
- Catch2 test target scaffold

Still in progress:

- Full legal move generation
- Perft validation
- Evaluation function
- Search
- Transposition table
- UCI protocol loop

Some files for these future modules already exist as placeholders so the project structure can grow into the planned architecture.

## Project Structure

| Path | Purpose |
| --- | --- |
| `src/types.h` | Core enums, square helpers, bitboard helpers, constants |
| `src/move.h`, `src/move.cpp` | 32-bit move encoding and UCI move helpers |
| `src/board.h`, `src/board.cpp` | Board state, FEN, make/unmake, attack checks, Zobrist update |
| `src/attacks.h`, `src/attacks.cpp` | Piece attack tables and sliding attack generation |
| `src/zobrist.h` | Zobrist hash declarations |
| `src/main.cpp` | Small CLI smoke test that initializes a board and prints FEN |
| `tests/` | Catch2 test target scaffold |

## Move Representation

Moves are packed into a 32-bit integer for fast copying, storage, and lookup during future move generation and search.

| Bits | Field | Description |
| --- | --- | --- |
| 0-5 | To square | Destination square, 0-63 |
| 6-11 | From square | Origin square, 0-63 |
| 12-15 | Promotion piece | Promotion piece type |
| 16-19 | Moving piece | Piece being moved |
| 20-23 | Captured piece | Captured piece, if any |
| 24-27 | Move flag | Quiet, capture, castling, en passant, promotion, etc. |

Supported move flags:

- Quiet move
- Capture
- Double pawn push
- King-side castling
- Queen-side castling
- En passant
- Promotion
- Promotion capture

The move module also supports UCI-style coordinate strings such as `e2e4`, `g1f3`, and `e7e8q`.

## Board State

The `Board` class currently tracks:

- Piece placement
- Side to move
- Castling rights
- En passant square
- Halfmove clock
- Fullmove number
- Zobrist key

It supports loading a position from FEN and serializing the current state back to FEN. It also includes make/unmake move logic using an `Undo` record so future search can safely explore and restore positions.

## Attack Generation

The attack module includes:

- Precomputed knight attacks
- Precomputed king attacks
- Precomputed pawn attacks
- Runtime sliding attacks for bishops, rooks, and queens

These attack helpers are used by board-level check and square-attack queries.

## Build

Requirements:

- CMake 3.20 or newer
- A C++17 compiler

Build the project:

```bash
cmake -S . -B build
cmake --build build
```

Run the current CLI:

```bash
./build/chesscli
```

Expected behavior right now: the executable initializes the standard starting position and prints the resulting FEN.

## Tests

The test target is configured with Catch2:

```bash
ctest --test-dir build
```

The current test files are scaffolds for upcoming FEN, make/unmake, perft, and search validation.

## Roadmap

Next milestones:

1. Implement pseudo-legal move generation.
2. Filter legal moves by king safety.
3. Add perft tests for move generation validation.
4. Implement a basic material/position evaluation.
5. Add alpha-beta search.
6. Add a transposition table backed by Zobrist keys.
7. Implement a proper UCI command loop for GUI compatibility.

## Goal

The goal is not only to build a working chess engine, but to understand how engines are structured internally: board representation, move encoding, search, evaluation, hashing, validation, and performance-oriented C++ design.

## Author

**Mehul Arya**
