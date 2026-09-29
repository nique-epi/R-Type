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

## Example

- ❌ **Before (wrong)**: using a CMake command introduced after 3.28 because it works with the local CMake → failure on a teammate's machine at the minimum required version.
- ✅ **After (right)**: check in the CMake docs when it was introduced, stay on the 3.28 equivalent, and cite the tested versions in the PR.
