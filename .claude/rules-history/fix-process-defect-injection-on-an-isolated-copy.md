# RULE: Inject a defect only into a copy the user cannot build — never into the shared working tree

## Context

Scrolling starfield background (branch `feat/scrolling-starfield`). After the first green build, red proofs were run by a script that, for each of 35 variants, rewrote one source file of `src/client/Background/`, rebuilt `client_background_tests` in `build/`, ran it, then restored the file.

## Mistake

The script ran in the user's own working tree and build directory, without telling the user. During the run the user launched `cmake --workflow --preset build` to look at the client: `build/` was reconfigured to Release without tests, the last 11 variants failed with "No rule to make target `client_background_tests`", and `r-type_client` was rebuilt at the root while defects were being written into the sources. The binary the user may have looked at could hold one of them. The sources were restored (compared to the approved code: 59 files identical) and the client was rebuilt from them, but the 11 variants had to be rerun.

## Root cause

The working tree and `build/` were treated as private instruments, while they are the user's: they can build from them at any moment, and a build directory is shared by every preset.

## Rule

1. Every temporary mutation of sources runs on an isolated copy (sources copied to the scratchpad, own build directory), never in the user's working tree or `build/`.
2. Measure that the copy builds before the first variant.
3. If no isolated copy is possible, tell the user before starting and ask them not to build meanwhile.
4. After any mutation run, compare each touched file with its reference.
5. A build directory changed by someone else during a run invalidates it from that point: warn the user, rebuild their binary from the restored sources, rerun the affected variants on the copy.

## Example

- ❌ **Before (wrong)**: 35 variants injected into the user's checkout; the user builds meanwhile; 11 variants lost, a client binary possibly built with a defect.
- ✅ **After (right)**: variants run in `<scratchpad>/red-proofs/` with its own build directory; the user's tree is never written.

## Update (2026-10-17) — a sync that broke the copy's vcpkg tool

### Context
The isolated copy of the client animations was synced from the tree before the clang-tidy fixes were rebuilt.

### Mistake
The sync (`rsync -a --delete`) included `vcpkg/`. The sandbox refused to write inside `vcpkg/downloads/tools/cmake-4.4.0-osx/…/CMake.app` (`Operation not permitted`), so the copy kept a CMake without its binary. The copy built until a `CMakeLists.txt` changed; its configure then failed with "expected this path to exist after extracting cmake". Copying the folder again failed the same way.

### Root cause
The sync copied a folder the copy never needs to change, and its errors were read as noise because the build that followed passed.

## Update (2026-10-18) — A path left out of the copy is named whole

### Context

Server half of the player inputs (branch `feat/keep-newest-player-input`). Before the red proofs of `PlayerInputReceiver`, the sources were copied to `<scratchpad>/red-proofs/source/` with `git ls-files -z --cached --others --exclude-standard | grep -z -v -E '^(vcpkg|docs/)' | rsync --files-from=-`, and the worktree's `vcpkg` submodule was linked in.

### Mistake

`^(vcpkg|docs/)` was meant to leave out the submodule entry `vcpkg`, but it also matched `vcpkg.json`, the manifest of the dependencies. `diff -rq` on `src/` and `tests/` said the copy was identical, so the configure was started; it failed on `Could not find a package configuration file provided by "asio"`, because vcpkg had no manifest and installed nothing. Measuring the copy before the first variant caught it: no variant had run.

### Root cause

The pattern was a prefix, not a whole path, and the check of the copy compared only the folders that hold code, not the root files the build reads.

### Rule

Each exclusion pattern matches a whole path (`^vcpkg$`, `^docs/`), and the root of the copy is compared, before configuring, with the root entries of the same tracked-and-untracked list it was made from (`git ls-files --cached --others --exclude-standard`): a plain `git ls-files` lists tracked paths only.

### Example

- ❌ **Before (wrong)**: `grep -v -E '^(vcpkg|docs/)'` → `vcpkg.json` missing, configure fails on `asio`.
- ✅ **After (right)**: `grep -v -E '^(vcpkg$|docs/)'`, root of the copy compared with the repository's root before configuring.

## Update (2026-10-19) — a sync that deleted the copy's vcpkg link

### Context
The isolated copy of the waiting room had its `vcpkg` as a symbolic link to the tree's submodule, and was resynced after three files were formatted.

### Mistake
The sync excluded `/vcpkg/`, with a trailing slash. In the copy, `vcpkg` is a link, not a folder, so the pattern did not match it and `--delete` removed it. The next `format-check` reconfigured the copy and failed: "include could not find requested file: …/ci-copy/vcpkg/scripts/buildsystems/vcpkg.cmake".

### Root cause
The exclude pattern of the rule was written for a copy holding no `vcpkg` at all; a pattern ending in `/` matches folders only.

### Rule
When the copy's `vcpkg` is a link, exclude it as `--exclude=/vcpkg`, and check `ls -l <copy>/vcpkg` after the sync.

## Update (2026-10-21) — a copy configured without the SDK compiled against SFML 2

### Context
Before the red proofs of the status band, the isolated copy was configured with `cmake -S <copy> --preset test -DVCPKG_MANIFEST_INSTALL=OFF`, its `vcpkg_installed` copied from the tree.

### Mistake
The copy's build failed in `AssetLibrary.cpp`, a file the change does not touch: `no matching constructor for initialization of 'sf::Texture'`. The compiler had opened `/usr/local/include/SFML/Audio/SoundBuffer.hpp`, an SFML 2 installed outside vcpkg.

### Root cause
The tree's cache holds `CMAKE_OSX_SYSROOT` set to the Xcode SDK, so its compile lines carry `-isysroot`; the copy's fresh configure left it empty. Without `-isysroot`, Apple clang searches `/usr/local/include` before the `-isystem` folder of vcpkg.

### Rule
Configure the copy with the tree's `CMAKE_OSX_SYSROOT`, compare one target's `CXX_FLAGS` in both builds before the first variant, and read an error inside an untouched library header as a wrong header first.
