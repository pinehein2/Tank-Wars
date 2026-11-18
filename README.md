# Project 2: Tank War
By Pine Hein Swe and Platon Shablyko

## Compiling and Running

```
g++ -std=c++17 -Wall -Wextra -o tankwars main.cpp Entity.cpp Location.cpp Map.cpp MoveAction.cpp EntityController.cpp PrintUI.cpp TankWars.cpp
./tankwars [options]
```

Options:

```
-h | --help                          Print this help message and exit.
--log-file <file>                    Log the game process to a file. (Default: tankwar.log)
-m <mode> | --mode=<mode>            Specify the game mode (PVP/PVE/DEMO). (Default: PVP)
-p <point> | --initial-life=<point>  Specify the initial life points of the tanks. (Default: 5)
```

On startup, each player is prompted for a spawn position and facing direction in the
form `x y DIRECTION`, e.g. `5 5 NORTH`, where `x` and `y` are 1-indexed (1-20) and
`DIRECTION` is one of `NORTH`, `EAST`, `SOUTH`, `WEST`.

## Architecture

The project follows the Layered Architecture Pattern, split into four layers. Lower
layers never call into higher layers.

1. **Data layer** — `Entity`, `Location`, `Map`
   Pure state holders with no game rules. `Entity` (base class for `Tank` and
   `Bullet`) tracks position, direction, health, and identity. `Location`
   represents a single map cell and whether it currently holds an entity, is
   selectable, or is active (inside the current, possibly shrunk, play zone).
   `Map` owns a grid of `Location`s and can shrink the play zone.

2. **Logic layer** — `MoveAction`, `TankController` (and its subclasses
   `PlayerTankController` / `AITankController`), `BulletController`
   Implements the actual game rules: generating legal moves, executing a move,
   firing bullets, moving bullets and resolving collisions, and applying
   out-of-zone damage. `PlayerTankController` reads a move from stdin;
   `AITankController` picks a random legal move.

3. **Orchestration layer** — `TankWars`
   Drives a full match: prompts for initial tank setup, resolves each turn in
   order (move → shoot → move → shoot → collision check → move bullets →
   shrink zone → out-of-zone damage → win/lose check), and owns the `Map` and
   both `TankController`s.

4. **Presentation layer** — `PrintUI`
   All console output and log-file writing (map rendering, turn banners,
   status lines, announcements) lives here, so the orchestration layer never
   calls `cout`/writes the log file directly.

`main.cpp` sits above all layers: it parses command-line options and drives the
turn loop by repeatedly calling `TankWars::playTurn()`.

## Object-Oriented Design

- `Entity` (abstract-ish base) → `Tank`, `Bullet`: shared position/health/direction
  state and behavior (`hurt`, `updatePosition`, etc.).
- `TankController` (base) → `PlayerTankController`, `AITankController`: shared
  turn logic (compute legal moves, fire, take zone damage); the subclasses only
  differ in how a move is chosen (`getInput`).
- `BulletController`: not tied to a single bullet instance — it scans the map
  each turn and advances every live bullet.
- `MoveAction`: a small value type describing one legal move (destination +
  resulting direction) and how to apply it to an `Entity`.
- `Map` / `Location`: composition — `Map` owns an array of `Location`s.

## I/O Format

- **stdout**: turn-by-turn banners, the rendered map (`-` = outside the current
  zone, ` ` = empty, `A`/`B` = tank, `*` = bullet), player status lines, and
  announcements (moves, hits, zone shrink, game result).
- **Log file** (`tankwar.log` by default, or `<name>.log` via `--log-file`):
  append-only record of each game — timestamp, mode, initial HP, spawn
  positions/directions, and the same win/loss/draw announcements shown on
  stdout (used for later review rather than full turn-by-turn detail).
- **stdin**: initial spawn prompts (`x y DIRECTION`), then, each turn, a
  numbered choice (`1`-`3`) from the currently offered moves for any
  human-controlled tank.

## Known Design Notes

- Tanks always stay within the original 20x20 board — a move that would leave
  the board is never offered as a legal option, and the initial spawn prompts
  and the AI's own spawn selection are both range-checked the same way.
- The play zone can still shrink every 16 turns (rule 2.4.2), and a tank
  caught inside the board but outside the current shrunk zone takes 1 HP of
  zone damage per turn, same as before.
- A tank is allowed to move directly onto the cell the other tank currently
  occupies. If both tanks end up on the same cell after a move, that is
  checked immediately as a crash (rule 2.5.3) and resolved before the mover's
  own shot is fired.

## Testing

`tests/test_same_position.txt` and `tests/test_invalid_range.txt` are sample
stdin files (same format as the spec's sample input) exercising spawn
validation in PVP mode:

```
./tankwars -m PVP -p 5 < tests/test_same_position.txt
./tankwars -m PVP -p 5 < tests/test_invalid_range.txt
```

`test_same_position.txt` has Player 2 try to spawn on Player 1's exact cell
first (rejected), then a valid different cell. `test_invalid_range.txt`
exercises an out-of-range coordinate and an invalid direction string before a
valid spawn. AI-controlled tanks (PVE's opponent, both tanks in DEMO) pick
their own random unoccupied spawn automatically and need no stdin for that
step -- try `./tankwars -m DEMO -p 5 < /dev/null` to see a DEMO game run with
zero input.

## Bug fixes from initial implementation

While integrating, the following issues were found and fixed:
- `--mode=PVP` was rejected as an invalid argument.
- A stray `argc > 3` check rejected valid combinations of options.
- `--log-file foo.log` produced `foo.log.log`.
- Bullets fired on a turn didn't start moving until the following turn.
- Tank-vs-tank collision (rule 2.5.3) was unimplemented.
- Tanks could not leave the initial 20x20 board (rule 2.4.1).
- `BulletController::tick` stopped processing the whole map after the first
  bullet hit that turn, silently skipping every other bullet in flight.
- Several `Bullet` allocations were never freed (memory leaks) when a bullet
  hit something immediately on spawn, or left the board, or two bullets
  collided.
- `AITankController` re-seeded `rand()` with `time(0)` on every move.
- Added destructors (`~TankController`, `~Map`, `~TankWars`) so game objects
  are properly freed at program exit; verified leak-free with
  AddressSanitizer/LeakSanitizer.
- Player 2 could previously spawn on the exact same cell as Player 1; spawn
  input is now validated (range, direction, and not-already-occupied) with
  re-prompting on invalid input.
- AI-controlled tanks (PVE's opponent, both tanks in DEMO) previously still
  required a human to type their spawn position from stdin; they now pick
  their own random, unoccupied spawn automatically.
