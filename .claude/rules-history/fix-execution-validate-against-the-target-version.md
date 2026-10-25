# History: validate against the version the target runs

Rule imported from another project when R-Type started: its original incident belongs to another repository.

## Update (2026-10-06) — latest stable release read on the official source

### Context
Plan for scaling the playfield to the client window, built on the SFML view, viewport and resize event.

### Mistake
The plan cited the SFML API from the pinned version only (3.0.2, read on the tag of the official repository). It did not say which release was the latest stable one, nor whether the API it relied on had changed since. The user asked to go to external sources for the latest stable versions.

### Root cause
"Validate against the target version" was read as "the pinned version is the only one worth reading". The pinned version says what compiles today; it does not say whether the code is about to be outdated, which is what the user wanted to know before approving.

### What was done
The latest stable releases were read on the official repositories (SFML 3.1.0, GoogleTest 1.18.0), the headers and the source file the plan uses were compared between SFML 3.0.2 and 3.1.0 (identical for what the plan uses), and both versions were written in the plan. The user then confirmed that no version was to be changed.

## Update (2026-10-14) — a system setting moved by a newer release

### Context
The title screen keeps its squadron still when the system asks for less motion. `systemPrefersReducedMotion()` reads macOS "Reduce motion", the Windows animation setting, and on Linux the GNOME key `enable-animations` through `gsettings`.

### Mistake
The three readings were written from the API reference of each setting (macOS SDK header, Microsoft documentation, GNOME schema), without reading how established projects read them nor where the latest release of each system stores them. The user asked to redo the work from external implementations. Firefox, Chromium, the gpui crate and the GNOME and KDE portal backends showed that since GNOME 50 the accessibility switch writes `org.gnome.desktop.a11y.interface reduced-motion`, while `enable-animations` is kept as a performance toggle: the Linux code misses the player's request on current GNOME, and ignores KDE entirely.

### Root cause
An API reference says how a setting is read, not whether it is still the one the user changes. The existing subsection asked for the latest release of a library, and was not applied to the operating systems the code reads.

### What was done
The macOS and Windows readings were confirmed identical to Firefox and Chromium. The Linux defect and the options to fix it (the portal key first, the old key as a fallback) were reported to the user before any code change.

## Update (2026-10-24) — a tool version copied from the rule's example

### Context
Gates of the client change that removes the entities no world state lists anymore: a fresh build of the test preset, 354 objects, 0 warnings, 1713 tests passing.

### Mistake
The result was noted in `.context/context.md` as "AppleClang 17". Nothing had printed that number: it is the one of point 3 of this rule ("build green under AppleClang 17 and CMake 4.1"). `c++ --version`, run later for the pull request, printed Apple clang 21.0.0. The slip was caught before the pull request was written.

### Root cause
Point 3 asked to state the version and showed one as an example; the example's number was taken for a fact about the machine instead of being read from the compiler.

### What was done
The note was corrected from `/usr/bin/c++ --version`, the compiler the build's `CMAKE_CXX_COMPILER` names, and `cmake --version`, the pull request states Apple clang 21.0.0 and CMake 4.3.2, and the example of point 3 no longer carries a number.
