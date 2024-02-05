### WDL Trainer
---

This project is a WIP trainer for Eval Normalisation models for chess engines.

Data is in the following format:
```
move score result
```
where ``move`` is the current game's ply count, the ``score`` is the result of a search, from the perspective of white to move, and ``result`` is the result of the game, from the perspective of white to move. ``0`` is a victory for black, ``0.5`` is a draw, and ``1`` is a victory for white.

Training is planned to use gradient descent, however it has yet to be implemented.
