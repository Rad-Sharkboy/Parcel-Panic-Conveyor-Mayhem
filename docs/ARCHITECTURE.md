# How the game works

The code has three parts: **conveyor movement**, **parcel tracking**, and **game/graphics**. The first two work without Raylib.

## A parcel's journey

1. Create a parcel record in the **hash table**.
2. Put its ID in an input **queue**.
3. When the entrance is free, remove that ID from the queue and place the parcel on the belt.
4. Move it through the chosen junctions.
5. At the correct dock, put its ID and deadline in the **min-heap**.
6. The shared loader removes the earliest-deadline parcel and awards points.

Wrong docks, broken fragile parcels and missed deadlines cause a failure. Each parcel is scored or penalized once.

## Movement and undo

The game remembers which cells were occupied before a movement step. Each empty target can be taken by only one parcel. This prevents two boxes entering the same cell.

While paused, changing a junction pushes its old setting onto the **stack**. Z pops that setting and restores it. The next game step clears the history, so undo only reverses planning changes.

## Drawing and timing

Game rules advance every **0.22 seconds**. Raylib draws between those steps, moving the visible boxes smoothly toward their next cells. Pause and F1–F3 freeze the factory.

`Game.cpp` handles input and timing. `Renderer.cpp` and `Screens.cpp` draw the game. `Ui.cpp` contains shared drawing helpers and font loading. Hovering a parcel reads its current record into a fixed sidebar card.

Restart and level skipping clear the current run. Fonts stay in `assets/fonts` beside the EXE.
