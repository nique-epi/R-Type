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
