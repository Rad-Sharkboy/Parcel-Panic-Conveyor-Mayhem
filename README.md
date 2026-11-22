# Parcel Panic: Conveyor Mayhem 📦

**Route parcels. Beat deadlines. Keep the factory moving.**

Parcel Panic: Conveyor Mayhem is a **C++17 and Raylib arcade game** built as a university Data Structures and Algorithms project. It runs on **Windows 11** and is developed using **VS Code and MinGW-w64**.

Switch conveyor junctions to send parcels to matching delivery docks. Build combos, avoid jams, protect fragile freight, and survive increasingly busy shifts.

**V2** adds a clearer shared loading bay, live data-structure inspection, route previews and improved shift briefings/start guide.

## 🎮 Features

- Four playable levels with different conveyor layouts.
- Red, blue and yellow destinations, express deadlines, fragile parcels and damaged parcels for recycling.
- Emergency brakes, rush waves, combos, scoring and victory/failure screens.
- Pause to plan routes and undo junction changes.
- Moving tracks, route previews, input staging and a shared loading bay with a countdown.
- A separate live DSA inspector with single-tick stepping and real operation captions.
- F1–F3 clearly labeled sample demonstrations using the custom data structures.
- Instant F4 level skipping.

## 📚 DSA concepts

| Structure | Implementation | Gameplay role |
|---|---|---|
| Circular queue | Dynamic array, front/rear indices, wraparound | Incoming parcel buffers |
| Min-heap | Dynamic array, deadline/sequence ordering | Earliest-deadline dispatch at the shared loader |
| Hash table | Linear probing, tombstones and rehashing | Parcel tracking by unique ID |
| Linked stack | Singly linked nodes | Undo paused junction changes |

These structures are implemented from scratch with ordinary arrays, linked nodes, destructors and deep copies. No STL containers replace them. Queue operations and stack push/pop are O(1); heap operations are O(log n), with O(n) growth; hash lookup is expected O(1), worst O(n). Full details are in [COMPLEXITY.md](docs/COMPLEXITY.md).

## 🧩 Modular design

The game is split into three modules, with each part responsible for a different side of the factory:

- **Conveyor and routing:** handles belt movement, junction switching, input queues, emergency brakes and stack-based undo.
- **Parcel tracking and dispatch:** manages the hash-table registry, parcel status, deadlines, heap-based loading and scoring.
- **Game and presentation:** brings the modules together through the game loop, four level layouts, menus, controls, animations and DSA demonstrations.

The simulation works independently of Raylib rendering. Visual changes can be made without rewriting dispatch logic, and conveyor rules can be refined without rebuilding the menu system. Shared parcel definitions keep the modules consistent, while `include/Config.h` keeps gameplay tuning in one place.

## ✨ V2 highlights

- Shared loading-bay staging makes the next shipment, waiting parcels and dispatch timing visible.
- Live queue, heap, hash-table and stack inspection lets players read real gameplay state and advance one tick at a time.
- Route previews, parcel hover cards, clearer input staging and skippable briefings make planning easier to understand.
- All four levels retain their custom queue, heap, hash-table and stack implementations.

## 📥 Download and play

To play without building, open [Releases](https://github.com/Rad-Sharkboy/Parcel-Panic-Conveyor-Mayhem/releases) and download the latest **`ParcelPanic-Windows-x64.zip`** from the Assets section. Extract the whole ZIP, open the `ParcelPanic` folder and run **`ParcelPanic.exe`**. Keep its **`assets`** folder beside the EXE. No compiler or editor is needed to play.

This source folder contains the V2 loading-bay and inspector improvements. To build or edit the code yourself, follow the Windows instructions below.

## ⚙️ What you need — Windows

1. **VS Code** with Microsoft's **C/C++** and **CMake Tools** extensions. VS Code will recommend them when you open the folder.
2. **MinGW-w64**: `gcc`, `g++` and `mingw32-make` available on PATH. For F5 debugging, also install `gdb`.
3. **CMake 3.22 or newer**, added to PATH during installation.
4. Internet access for the first build. CMake downloads Raylib automatically; no manual Raylib or vcpkg setup is needed.

If the same MinGW compiler already builds your PC-Builder-Tycoon project, you can use it here too. **Visual Studio is not required.**

### If you do not have MinGW yet

Install [MSYS2](https://www.msys2.org/), open its **UCRT64** terminal, and run:

```text
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-gdb
```

Add the installation's `ucrt64\bin` folder to your Windows user PATH (normally `C:\msys64\ucrt64\bin`). Install [CMake](https://cmake.org/download/) and choose its **add to PATH** option. Close and reopen VS Code afterward.

In a new VS Code terminal, these commands should show installed versions:

```powershell
g++ --version
mingw32-make --version
cmake --version
```

## 🚀 Download, build and play

### 1. Download

Click **Code → Download ZIP** on GitHub and extract the ZIP. Open the extracted project folder in **VS Code**.

### 2. Build

Press **Ctrl+Shift+B**, or run this in the VS Code terminal:

```powershell
.\build.bat
```

The script downloads Raylib on the first build, compiles the game and copies the included fonts into `build/assets`. Wait for **“Build complete.”**

The `build` folder is generated: you can delete it at any time and run the script again for a fresh build. The unused MOD music decoder is disabled to avoid the GCC 15 warning from Raylib's `jar_mod.h`; game warnings remain enabled.

The Windows workflow builds and packages the game for releases. The local verification harness used during V2 development is intentionally not included in the public source folder.

### 3. Play

Double-click **`ParcelPanic.exe`** inside the generated **`build`** folder, or run:

```powershell
.\build\ParcelPanic.exe
```

Keep the generated **`assets`** folder beside **`ParcelPanic.exe`**, including when sharing the game. The included Tomorrow fonts need no separate installation.

You can also choose **Terminal → Run Task → Run ParcelPanic**. To debug, press **F5**; the supplied VS Code configuration builds Debug and uses GDB. Once the game has keyboard focus, its F5 key restarts the current level.

After changing a `.cpp` or `.h` file, save it and press **Ctrl+Shift+B** again. Build the whole project; the single-file “Run Code” command will not link the other source files or Raylib.

## 🕹️ Controls

| Input | Action |
|---|---|
| Left click | Switch a cyan junction |
| Right click | Temporarily brake a conveyor cell; shared cooldown |
| Hover a parcel | Inspect its destination, type, status and remaining time |
| Space / Esc | Pause/resume planning |
| Z while paused | Undo a junction change |
| I during a shift or on the results screen | Open/close the live inspector; closing leaves planning paused |
| Tab / click a tab in the inspector | Change structure view |
| Enter in the inspector | Advance one real simulation tick |
| Digits, Backspace, then L in the hash tab | Enter and look up a parcel ID |
| J / S / Z in the stack tab | Select a junction / switch and push / undo and pop |
| Left / Right in the inspector | Previous / next page of entries |
| H | Switch between Help and Factory info |
| Q / click the input queue heading | Inspect the next live input queue |
| R / F5 | Restart the level |
| F1 / F2 / F3 | Open queue / heap / hash demonstrations |
| Enter / Backspace / Esc in a demo | Next operation / reset / close |
| F4 / Shift+F4 | Next / previous level immediately |

F4 wraps from level 4 to level 1. Shift+F4 wraps backward. Skipping resets the run and awards no victory. You can also choose any level from the title screen.

The F1–F3 practice demos open with **three sample entries already inserted**. You can also click their buttons along the bottom. Click **Next operation** or press **Enter** to continue; **Restart sample / Backspace** restores the starting example.

The loading bay shows the next shipment, waiting parcel deadlines, the next loading slot and completed shipments. Only the highlighted first tile is the next dispatch; the other visible tiles are staging entries, not a sorted list. Input staging shows parcels front-first, with full/blocked feedback and **Last to belt**. Hover a moving parcel or staging tile to read its real hash-table record. Hover a junction to preview the selected cyan and alternative gold paths. Each shift starts with a skippable briefing.

Press **I** to see the four structures from your current run. Browsing their slots, buckets and nodes leaves them as they are. The **Tick +1**, **Switch** and **Undo** controls act on the run, and each tick clears the undo history. Use **F1–F3** to step through prepared examples, such as a full queue, equal heap deadlines and a hash-table collision with deletion.

## 🏭 Four levels

| Level | Main challenge | Arrivals | Required deliveries | Score target |
|---|---|---:|---:|---:|
| Sort School | Two colors, two input lanes, introductory routing | 20 | 12 | 1,200 |
| Rush Junction | Three colors, merging lanes and rush waves | 38 | 23 | 2,300 |
| Fragile Freight | Crusher bypass, express/fragile parcels and recycling | 48 | 29 | 2,800 |
| Mayhem Mode | Vertical sorter, all parcel types and a slower loader | 60 | 36 | 3,600 |

Sort School stays relaxed. Rush Junction adds a visible shared-loader backlog. Fragile Freight introduces fragile, then express, then damaged parcels at configurable arrival thresholds. Mayhem Mode combines the routes and tighter traffic. Loader intervals and teaching thresholds live in `include/Config.h`; `MaxParcels` now follows the largest of all four configured totals.

Match parcel letters to docks: **R**, **B**, **Y**, and **X** for damaged/recycling. **F** means fragile; **!** means express. Reach both the quota and score target. Wrong docks, missed deadlines and broken fragile parcels break your combo. A full jam meter ends the shift.

---

## 🤝 Contributors

- **[@Rad-Sharkboy](https://github.com/Rad-Sharkboy)** — Module 1 (Conveyor System: Circular Queue, Linked Stack undo mechanics, core level routing) and shared Engine/Renderer architecture.
- **[@Sabtainn](https://github.com/Sabtainn)** — Module 2 (Parcel Tracking and Dispatch: Open-Address Hash Table, Deadline Min-Heap, DispatchSystem lifecycle) and shared work on Module 3 (Simulation tick coordination, Live DSA Inspector, and UI/Screen presentation).