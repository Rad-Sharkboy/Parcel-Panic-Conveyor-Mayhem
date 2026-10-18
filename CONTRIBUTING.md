# Contributing

This project uses **Windows 11, VS Code, MinGW-w64 and CMake**. Follow the README to install the tools. Raylib is downloaded automatically.

1. Clone the repository or download its ZIP, then open the folder in VS Code.
2. For Git contributions, update main with `git pull --ff-only`, then create a branch such as `git switch -c feature/routing-help`.
3. Make focused changes. Keep the four DSA implementations manual and the simulation independent of Raylib. Discuss interface changes in `docs/INTERFACES.md`.
4. Press **Ctrl+Shift+B** or run `.\build.bat`. This builds the game. Press **F5** for the supplied GDB debugging configuration.
5. Play affected levels. Check pause/undo, braking, F1–F3 demos, F4 skipping, F5 restart and results where relevant.
6. Inspect your diff and commit only intentional source/documentation changes. Include `.vscode` project settings; exclude generated `build`/`out` folders, downloaded Raylib, local credentials and binaries.
7. Push your feature branch and open a pull request. Explain what changed and which checks you actually ran. Ask the other member to review it and merge only after review and passing Windows CI.

Member 1 is planned to maintain routing, queue and stack; Member 2 tracking, hash and heap; both presentation, integration and documentation. Both members should understand the whole game. The initial prototype was generated with Codex; planned ownership does not imply human authorship.

For a playable release, build Release with `.\build.bat`, test `build\ParcelPanic.exe` on Windows 11, and attach that EXE to the GitHub Release. Do not claim a hosted workflow or Windows run passed until it actually does.
