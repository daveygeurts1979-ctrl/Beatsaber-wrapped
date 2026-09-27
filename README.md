# Beat Saber Wrapped

Spotify-Wrapped-style monthly/yearly Beat Saber play stats (total minutes,
sessions, top 10 songs). Built for game version `1.40.8` on the Scotland2
Quest modding stack (beatsaber-hook + BSML + QPM).

## Layout — everything is flat on purpose

Every source file sits directly in this folder (no subfolders except the
hidden `.github/workflows/`). This is deliberate: it makes uploading the
project through GitHub's mobile "Add file → Upload files" screen foolproof —
just select every file at once and drop them all in the repo root. No folder
structure to accidentally lose.

Files:
- `main.cpp` — mod entry point, installs hooks, adds the "Wrapped" main-menu button
- `WrappedManager.cpp` / `.hpp` — play-log storage + monthly/yearly aggregation (version-independent)
- `GameplayHooks.cpp` / `.hpp` — detects when a song starts/ends (has the version-sensitive hook signatures — see below)
- `WrappedFlowCoordinator.cpp` / `.hpp`, `WrappedViewController.cpp` / `.hpp` — the BSML recap screen
- `Logging.hpp` — shared logger
- `qpm.json`, `CMakeLists.txt`, `mod.template.json`, `cover.png` — build/package config
- `.github/workflows/build.yml` — builds a `.qmod` automatically via GitHub Actions

## Upload checklist

Make sure **all 13** of these end up in the repo:
`main.cpp`, `WrappedManager.cpp`, `WrappedManager.hpp`, `GameplayHooks.cpp`,
`GameplayHooks.hpp`, `WrappedFlowCoordinator.cpp`, `WrappedFlowCoordinator.hpp`,
`WrappedViewController.cpp`, `WrappedViewController.hpp`, `Logging.hpp`,
`qpm.json`, `CMakeLists.txt`, `mod.template.json`, `cover.png`, plus the hidden
`.github/workflows/build.yml`. If any are missing, the Actions build will fail
immediately — check the repo's file list against this before re-running it.

## Why some lines say "VERIFY AGAINST YOUR CODEGEN"

Beat Saber's game types get compiled into slightly different C++ headers per
game version. The hook signatures in `GameplayHooks.cpp` and the main-menu
button hook in `main.cpp` are written to match what's been stable across
recent Quest builds, but if `qpm restore` pulls headers that differ slightly,
the build will fail at that exact line — check the parameter list against the
header it names and adjust.

## Build via GitHub Actions (no PC needed)

1. Push/upload every file above into a GitHub repo.
2. Go to the **Actions** tab — the "Build QMOD" workflow runs automatically.
3. If it's green, download the `BeatSaberWrapped-qmod` artifact from the
   bottom of that run's page — that's your `.qmod` file.
4. If it's red, open the failing step and send me the error.
5. Install the `.qmod` onto your Quest via MBF.
