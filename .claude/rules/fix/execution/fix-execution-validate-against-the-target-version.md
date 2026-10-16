---
description: Validate code, config or artifacts against the version the target runs (minimum CMake, CI compilers, pinned dependencies) — never against the local version lying around
trigger: always_on
---

# RULE: Validate against the VERSION the target runs — never against the local version lying around

## Rule

1. **Establish the version the target runs before validating**: the declared minimum (`cmake_minimum_required(VERSION 3.28)`, C++20), the CI compilers (GCC on `ubuntu-latest`, MSVC on `windows-latest`), the dependencies **pinned** by vcpkg (baseline in `vcpkg.json`: SFML 3, Asio, GoogleTest).
2. **For every "recent" feature** (CMake command, preset field, C++20/23 standard library feature, SFML API), check when it was introduced and whether all three compilers support it before adopting it. A recent `std::` feature that compiles with AppleClang may be missing from GCC 13 or MSVC.
3. **State the validated version in the report**: "build green" means nothing; "build green under AppleClang 17 and CMake 4.1, Linux/Windows CI to be confirmed" is a result. Untested parity is stated as a residual risk.
4. **The library headers used are the ones installed by vcpkg**, not those of a system install of another version (Homebrew, apt).

### Read the latest stable release on the official source too, and say how it differs

1. **Before relying on a library API in a plan**, read it on the library's official source (its repository at the release tag, its official documentation) for **both** the pinned version and the latest stable release, and state the two version numbers.
2. **Compare what the plan uses between the two**: say whether it is identical, and list what differs. A tag of the official repository is a source; memory is not.
3. **A newer release is information, never a mandate**: no pinned version, baseline or submodule is changed unless the user asks for it.

### A system setting is read where the latest release of each system stores it

1. **Before reading an operating system setting** (an accessibility preference, the theme, the locale), find where the **latest stable release of each system** stores it, and read how at least two established implementations read it today: a browser engine (Firefox, Chromium), a toolkit (GTK, Qt), a desktop portal backend. Cite the source files read, not only the API reference of the setting.
2. **A setting that a newer release moved or redefined makes the code reading the old place a defect**: say which release changed it, read the new place first and the old one only as a fallback.
3. **Prefer the cross-desktop interface** when the platform has one (the freedesktop Settings portal on Linux) over the settings of a single desktop.

## Example

- ❌ **Before (wrong)**: using a CMake command introduced after 3.28 because it works with the local CMake → failure on a teammate's machine at the minimum required version.
- ✅ **After (right)**: check in the CMake docs when it was introduced, stay on the 3.28 equivalent, and cite the tested versions in the PR.
- ❌ **Before (wrong)**: the Linux reduced motion check reads `org.gnome.desktop.interface enable-animations` through `gsettings`, from the GNOME schema alone; since GNOME 50 the accessibility switch writes `org.gnome.desktop.a11y.interface reduced-motion` and `enable-animations` is a performance toggle, so the player's request is missed.
- ✅ **After (right)**: read Firefox, Chromium and the GNOME and KDE portal backends → read the portal key `org.freedesktop.appearance reduced-motion` first (GNOME 50 and KDE answer it), `enable-animations` only when the portal has no such key.
