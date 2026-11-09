# Context: RTY-257 (coroutines per body, spawn/destroy, budget per tick)

## Where we are
- Branch `feat/script-coroutines`, from `origin/feat/game-design`. WIP commit, pushed, NO PR (Nolan out of credits).
- Game Design PRs target `feat/game-design`, never `main`. RTY-252 (bench) merged there (#308).
- WIP: the code was building before the last edit (budget warning moved); that edit was interrupted. See "State of the last edit" below. NO new test written yet.

## Decisions (Nolan: per-tick budget included in this issue; RTY-252 comment dropped)
- One Luau thread per body, in `Pilot::memory` (`shared_ptr<IBehaviourMemory>`, shared_ptr because tests copy Pilot), so it dies with the body on every destroy path. `ScriptedRoutine` holds thread, ticksLeft, held intent, stopped.
- Script API: `behaviours.wait(ms, intent)` returns a fresh view when the wait ends; ms rounded to whole ticks, min 1 (`ms * 60 / 1000`, integer ticks, not a duration countdown). Function that returns restarts next tick (old scripts unchanged). Held intent drops one-time requests (startsAction, endsAction, jumpsTo) after the first tick. `coroutine.yield` directly, or wait outside the body's thread = failure.
- `behaviours.spawn(type, {x,y})` returns the network id (reserved at once via `World::reserveNetworkId`, `BodyPlacement::reservedNetworkId`); `behaviours.destroy(id)`; `view.id`. Deferred (add during forEach throws): `ScriptedBodies` (src/server/Spawning, implements `IScriptWorld`) applies them in `GameFlow` after the volleys, announces EntitySpawned, publishes EntityDestruction Removed. Limits 256/tick each. A ship can be neither created nor destroyed by a script.
- Budgets: `SCRIPT_STEP_BUDGET` 2 ms, `SCRIPT_TICK_BUDGET` 8 ms (`ScriptMachine::beginTick/hasTickBudget/resume`). Over tick budget: no step starts, bodies hold, retried next tick. Worst tick about 8 + 2 ms instead of 300 x 20 ms.
- A failing body is stopped for good and stays still; others go on; reported once per behaviour.
- Fixes of existing holes a script could reach: everything that allocates runs in `runProtected` (lua_cpcall), since an allocation refused outside a pcall ends the program; `intentAt` uses `lua_rawgetfield` (a `__index` loop would hang the server with no budget); `jumps_to` not finite refused (NaN breaks encoding).

## Found with the bench (reason for the last changes)
- A machine stall killed an innocent body (2 ms is wall time): the budget now needs `SCRIPT_OVERDUE_SAFEPOINTS` (1024) loops/calls past the deadline.
- 3000 waiting bodies hit 16 MiB through uncollected garbage: `beginTick` forces a full GC above 75 % of the limit.
- 300 waiting bodies, resumed every tick: median tick about 0.78 ms, scripts 0.28 ms, memory about 2.2 MB (1.3 MB non waiting). 3000: median 4.5 ms, 11.7 MB. 6000 fail: not enough memory (real limit of 16 MiB).

## Done on 2026-11-09 (second session)
- Last edit checked: builds with zero warnings (Apple clang 21), 422 scripting/spawning tests green.
- Spikes: NOT reproduced. 3 runs `--waiting`: worst 1.7 to 4.9 ms; 3 runs plain: worst 1.6 to 2.9 ms (Release, M4 Pro). Cause of the earlier 17-53 ms unknown (likely before the GC/safepoint changes, or machine load); not proven.
- `afc87bf8` test: tests/game/Scripting/BehaviourRoutineTest.cpp (8 tests: wait held, resume, memory released after destroy, failure and step budget stop one body, direct yield refused, spawn and destroy reach the world) + fixture, constants, double tests/doubles/RecordingScriptWorld. Red proofs run: hold removed -> HoldsTheIntent red; unref removed -> ReleasesTheMemory red.
- `51aab177` docs: scripting.md (wait/spawn/destroy/view.id, budgets, failure, waiting bench row), engine.md (step and tick budgets), project-layout.md (ScriptedBodies).

## Next steps
1. Nolan's decision on the failed body (below), then apply it (3 lines + change `LeavesTheBodyStillAndReportsAFailureOnce`).
2. Not covered yet: ScriptedBodies server tests (appear + announce, ship untouched, limits, detach on new world); bench tests for `--waiting`; one-time requests applied once. Decide with Nolan whether they go in this PR.
3. Reword the WIP commit `3922c8cb` before the PR (needs a force-push of this branch, ask Nolan).
4. Gates ONCE at PR time (Nolan: credits): full Debug workflow, format-check, clang-tidy on the changed files only, clang-query ten-method count on ScriptMachine and World.
5. `git rm --cached .context/context.md`, then PR `--base feat/game-design`, `Closes RTY-257`, English, draft, no AI mention.

## Open decision for Nolan
- A body whose script failed stays still for good (shipped behaviour kept). If it froze off-screen, the level could never end (boss stage waits for every enemy). Recommendation: remove it instead (`destroy(view.networkId)`, 3 lines, changes `LeavesTheBodyStillAndReportsAFailureOnce`).

## Build without touching build/
cmake -S . -B <scratch>/build-bench -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON -DVCPKG_MANIFEST_FEATURES=tests -DCMAKE_TOOLCHAIN_FILE=<main checkout>/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_INSTALLED_DIR=<main checkout>/build/vcpkg_installed -DVCPKG_MANIFEST_INSTALL=OFF -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=<scratch>/build-bench

## Sources
- https://luau.org/sandbox, https://luau.org/api/, Luau 0.729 VM/src (ldo.cpp, lstate.cpp, lapi.cpp)
- Unity StopCoroutine and EntityCommandBuffer docs, Roblox task library docs

## State of the last edit (budget warning moved to ScriptedBodies)
Applied in the files; not built since. It removes `reportOnce` from ScriptedBehaviour and logs the budget warning from ScriptedBodies.
