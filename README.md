# Parcel Panic: Conveyor Mayhem 📦

**Route parcels. Beat deadlines. Keep the factory moving.**

A C++ and Raylib game made as a university DSA project for **Windows 11**. Switch conveyor junctions, match parcels to docks, protect fragile boxes and avoid jams.

## 🎮 Download and play

1. Open [Releases](https://github.com/Rad-Sharkboy/Parcel-Panic-Conveyor-Mayhem/releases).
2. Download **ParcelPanic-Windows-x64.zip** from the newest development release.
3. Extract the ZIP, open the **ParcelPanic** folder and double-click **ParcelPanic.exe**.

Keep the **assets** folder beside the EXE. You do not need VS Code or a compiler to play. Development releases are work in progress; the game may change.

## 🕹️ Controls

| Input | Action |
|---|---|
| Left click | Switch a cyan junction |
| Right click | Brake a belt cell temporarily |
| Hover a parcel | Show its destination, type, status and time left |
| Space / Esc | Pause or resume |
| Z while paused | Undo a junction change |
| H | Help / Factory info |
| Q / click the queue heading | View another input queue |
| R / F5 | Restart the level |
| F1 / F2 / F3 | Queue / heap / hash demo; footer buttons work too |
| Enter / Backspace / Esc in a demo | Next operation / restart sample / close |
| F4 / Shift+F4 | Next / previous level, wrapping at either end |

F1–F3 open with **three sample entries**. Click **Next operation** to continue.

The live queue shows parcels waiting to enter a belt. Its colored boxes contain parcel IDs. **Last to belt** remembers the latest departure. The undo stack fills when you pause and switch a junction.

## 🏭 Four shifts

| Level | Challenge | Deliveries needed | Score target |
|---|---|---:|---:|
| Sort School | Learn the two-color routes | 12 / 20 | 1,200 |
| Rush Junction | Three inputs and rush waves | 23 / 38 | 2,300 |
| Fragile Freight | Crusher bypass and recycling | 29 / 48 | 2,800 |
| Mayhem Mode | All parcel types and a slower loader | 36 / 60 | 3,600 |

Match **R**, **B** and **Y** to their docks. Send **X** to recycling. **F** means fragile; **!** means express. Meet both the delivery and score targets. Wrong docks, broken parcels and missed deadlines break your combo.

## 📚 DSA and modular design

All four structures are written manually, using arrays or linked nodes. No prebuilt stack, queue, heap or map replaces them.

| Module | What it handles |
|---|---|
| Conveyor and routing | Circular input queues, belt movement and linked-stack undo |
| Tracking and dispatch | Hash-table parcel records and heap-based deadline ordering |
| Game and graphics | Four levels, input, menus, animation and practice demos |

The game rules and drawing code are separate, so changing a menu does not require rewriting the data structures.

## 🛠️ Build or edit in VS Code

Use **VS Code, MinGW-w64 and CMake 3.22+**. See [Windows setup](docs/WINDOWS_SETUP.md) if you need to install them. Visual Studio is not required.

Open the project folder in VS Code, then press **Ctrl+Shift+B** or run:

```powershell
.\build.bat
.\build\ParcelPanic.exe
```

The first build downloads Raylib and needs internet access. Rebuild after editing code. Use the whole-project build, not the single-file “Run Code” button. F5 in VS Code starts debugging; F5 inside the game restarts the level.

For class: [code guide and includes](docs/CODE_GUIDE.md), [how the game works](docs/ARCHITECTURE.md), [main functions](docs/INTERFACES.md), [complexity](docs/COMPLEXITY.md), [short report](docs/REPORT.md), and [team plan](docs/TEAM_PLAN.md).

---

*Made by me and Sabtain.*
