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

## Example

- ❌ **Before (wrong)**: 35 variants injected one by one into `src/client/Background/` of the user's checkout; the user runs `cmake --workflow --preset build` meanwhile, `build/` switches to Release, 11 variants fail with "No rule to make target", and `r-type_client` is rebuilt while a defect is in place.
- ✅ **After (right)**: copy the sources to `<scratchpad>/red-proofs/`, configure the test preset there with its own build directory, check it is green, run the variants there; the user's tree and `build/` are never written.
