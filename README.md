# Konane

Minimal text-based implementation of Konane (Hawaiian checkers) in C.

## Summary

Konane is a two-player abstract strategy game. This repository contains a compact implementation with:

- A bitboard-based 7x7 board representation
- Move generation (mandatory captures / multi-jumps)
- Simple AI using negamax with alpha-beta pruning
- Perft / benchmarking utilities for move-generation verification

## Files

- `src/` — C source files
  - `main.c` — entry point (starts the UI)
  - `game.c` — main game loop, opening removals, turn handling
  - `board.c` — bitboard utilities and print helpers
  - `move.c` — move encoding, generation and execution
  - `ai.c` — evaluator and search (negamax)
  - `perft.c` — perft / benchmark and diagnostic helpers
  - `ui.c` — minimal menu-driven UI
- `src/include/` — public headers for each module
- `Makefile` — simple build rules (produces `konane`)

## Build

Prerequisites: a C compiler (gcc/clang) and `make`.

On Linux/macOS:

```sh
make
```

On Windows (MinGW):

```powershell
mingw32-make
```

Build artifacts: `konane` (executable)

## Run

```sh
./konane
```

Choose from the menu:
- Play PvP (local human vs human)
- Play PvAI (human vs AI)
- Watch AIvAI
- Perft / Benchmarks (node counts, diagnostics)

## Notes & Design

- The board uses a 49-bit bitboard (lower bits of `uint64_t`) in row-major order.
- Initial removals: Black removes a corner or center; White removes an adjacent stone.
- Comments are intentionally concise; the code favors clarity over heavy documentation.

## Contributing

Small fixes and clarifications are welcome. Keep changes focused and add short, human-friendly comments when modifying behavior.
