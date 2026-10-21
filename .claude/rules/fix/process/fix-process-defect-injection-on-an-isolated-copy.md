---
description: Red proofs and any temporary source mutation run on an isolated copy, never in the working tree the user builds from
trigger: always_on
---

# RULE: Inject a defect only into a copy the user cannot build — never into the shared working tree

Why: red proofs rewrote sources of the user's working tree while the user built and ran the client from it; the binary they looked at could hold an injected defect.

## Rule

1. **Every temporary mutation of sources** (red proofs, a defect injected to calibrate a tool, an experiment to revert) runs on an **isolated copy**: a plain copy of the sources (tracked and untracked files, `build/` excluded) in the scratchpad, with its own build directory. Never in the user's working tree, never in its `build/`.
2. **Measure that the copy builds** (configure, build, tests green) before the first variant; a claim about that environment is measured there (see `fix-reasoning-delegated-invariant-must-be-computed.md`). Inside the copy, each variant is rebuilt as `fix-execution-zero-warnings-proven-on-a-recompiling-build.md` requires.
3. **If no isolated copy is possible**, tell the user before starting: what will change, for how long, and that nothing may be built meanwhile. Say when it is over.
4. **After any mutation run, prove the tree is back**: compare each touched file with its reference (the approved code, `git diff`), not just the exit code of the restore.
5. **A build directory reconfigured or rebuilt by someone else during a run** (`CMakeCache.txt`, binaries at the root changing) invalidates the run from that point: tell the user first that their binary may hold an injected defect, rebuild it from the restored sources, then rerun the affected variants on the isolated copy.

### The copy is synced without the dependency folders

1. **Sync the copy with `rsync -a --delete`, excluding `/build/` and `/vcpkg/`**: the sandbox cannot write inside the `.app` bundle of the CMake that vcpkg downloaded, so a sync that includes `vcpkg/` leaves that tool broken in the copy, and its next configure fails with "expected this path to exist after extracting cmake".
2. **When the copy's vcpkg tool is broken**, reconfigure the copy with `-DVCPKG_MANIFEST_INSTALL=OFF`: it reuses the packages already in its `build/vcpkg_installed/`, and nothing in the user's tree is touched.
3. **After every sync, compare the copy with the tree** (`diff -rq`, excluding `build`, `vcpkg` and `.git`): it prints nothing before the first variant.
4. **When the copy's `vcpkg` is a symbolic link to the tree's, exclude it as `--exclude=/vcpkg`, without the trailing slash**: a pattern ending in `/` matches folders only, so `--delete` removes the link, and the next configure fails on a missing `vcpkg/scripts/buildsystems/vcpkg.cmake`. Check `ls -l <copy>/vcpkg` after the sync.

### A path left out of the copy is named whole

1. **Each pattern that leaves a path out of the copy matches the whole path**: `^vcpkg$` for the submodule entry, `^docs/` for a folder, never a bare prefix such as `^vcpkg`, which also drops `vcpkg.json` and every root file that starts the same way.
2. **Before configuring the copy, compare its root with the repository's**: take the root entries of the same tracked-and-untracked list the copy was made from (`git ls-files --cached --others --exclude-standard`), less the paths left out on purpose; each one must be in the copy. A plain `git ls-files` lists tracked paths only, so an untracked root file missing from the copy would pass. A missing manifest shows up later as a dependency CMake cannot find, never as a missing file.

### The copy is configured with the tree's SDK

1. **On macOS, configure the copy with the `CMAKE_OSX_SYSROOT` of the tree's cache**: `-DCMAKE_OSX_SYSROOT="$(grep -E '^CMAKE_OSX_SYSROOT:' build/CMakeCache.txt | cut -d= -f2-)"`. A fresh configure may leave it empty; the compiler then gets no `-isysroot` and searches `/usr/local/include` before the `-isystem` folder of vcpkg, where an SFML 2 installed outside vcpkg is found first.
2. **Before the first variant, compare the compile flags of one target in both builds** (`CXX_FLAGS` in `flags.make`): any difference other than the source path is a different environment, never a code defect.
3. **An error inside a library header no change touched** (`no matching constructor for initialization of 'sf::Texture'`) **is read as the symptom of a wrong header first**: replay the compile with `-H` and read which file it opened.

## Example

- ❌ **Before (wrong)**: 35 variants injected one by one into `src/client/Background/` of the user's checkout; the user runs `cmake --workflow --preset build` meanwhile, `build/` switches to Release, 11 variants fail with "No rule to make target", and `r-type_client` is rebuilt while a defect is in place.
- ✅ **After (right)**: copy the sources to `<scratchpad>/red-proofs/`, configure the test preset there with its own build directory, check it is green, run the variants there; the user's tree and `build/` are never written.
- ❌ **Before (wrong)**: `rsync -a --delete` of the whole tree into the copy, `vcpkg/` included → "Operation not permitted" inside `CMake.app`, then the copy's configure fails when a `CMakeLists.txt` changes.
- ✅ **After (right)**: the sync excludes `/vcpkg/`; the broken copy is reconfigured with `-DVCPKG_MANIFEST_INSTALL=OFF`, then built and tested green before the variants.
- ❌ **Before (wrong)**: `git ls-files … | grep -v -E '^(vcpkg|docs/)'` to leave the submodule out of the copy → `vcpkg.json` is dropped too, the configure of the copy fails on `Could not find a package configuration file provided by "asio"`.
- ✅ **After (right)**: `grep -v -E '^(vcpkg$|docs/)'`, then the root of the copy compared with the root entries of `git ls-files --cached --others --exclude-standard` before configuring.
