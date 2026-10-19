# Working on the project

Use the [Windows setup](docs/WINDOWS_SETUP.md), then open the folder in VS Code.

1. Make one clear change at a time. Keep the DSA classes manual and the code close to the lab examples.
2. Run `.\build.bat` or press Ctrl+Shift+B.
3. Play the affected level. Check the controls or demo you changed.
4. Update the short docs if behavior changed. Commit with a message describing the actual change.

Keep build folders, tests and local screenshots out of the repository. Share the EXE with its assets through Releases. The planned team split is in [TEAM_PLAN.md](docs/TEAM_PLAN.md).

## Development releases

Pushing a new tag such as `v0.1.0-dev.2` runs the Windows build and publishes a GitHub **pre-release** with a playable ZIP. Use a new version tag for each release. Wait for the workflow to succeed before announcing the download.

The ZIP includes the EXE, fonts and a short read-me file. Windows gameplay should still be checked locally.
