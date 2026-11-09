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

## Next steps
1. Check the last edit (below) and build.
2. INVESTIGATE: `--waiting` runs showed worst ticks of 17 to 53 ms, non waiting 1.2 to 3.3 ms. Compare 10 runs of each; if it is inside `scripts`, suspect the GC with many threads. Not proven either way.
3. Write tests (none yet):
   - tests/engine/Scripting: resume after yield, step budget, tick budget and beginTick, thread dropped frees memory after full GC, runProtected turns a memory error into ScriptException.
   - tests/game/Scripting (new BehaviourRoutineTest + constants, double `tests/doubles/RecordingScriptWorld`): wait over several ticks, held intent, one-time requests once, return restarts, failure stops one body only, direct yield and wait outside fail, `__index` not run, NaN jump refused, postponed step resumes, spawn/destroy bad inputs.
   - tests/server/Spawning (ScriptedBodiesTest, into `server_weapon_system_tests`): body appears and is announced with the returned id, removal published, ship untouched, limits, new world detach, destruction during a wait frees the memory.
4. Bench: decide how to show the ticks that spent the script budget. Update its tests for `--waiting`.
5. Docs: `scripting.md` (wait, spawn, destroy, id, budgets, failure stops the body, new figures), `engine.md` (20 ms line), `project-layout.md` if needed.
6. Gates: full Debug tests, `format-check`, clang-tidy (llvm@20 with SDK flags, `-DINFINITY=__builtin_inff()`), `clang-query` ten-method count on ScriptMachine (at 10) and World. Then PR with `--base feat/game-design`, `Closes RTY-257`, English, draft, no AI mention.
7. Remove this file from the branch before the PR (`git rm --cached .context/context.md`): .context is ignored, it was force-added only to survive.

## Open decision for Nolan
- A body whose script failed stays still for good (shipped behaviour kept). If it froze off-screen, the level could never end (boss stage waits for every enemy). Recommendation: remove it instead (`destroy(view.networkId)`, 3 lines, changes `LeavesTheBodyStillAndReportsAFailureOnce`).

## Build without touching build/
cmake -S . -B <scratch>/build-bench -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON -DVCPKG_MANIFEST_FEATURES=tests -DCMAKE_TOOLCHAIN_FILE=<main checkout>/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_INSTALLED_DIR=<main checkout>/build/vcpkg_installed -DVCPKG_MANIFEST_INSTALL=OFF -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=<scratch>/build-bench

## Sources
- https://luau.org/sandbox, https://luau.org/api/, Luau 0.729 VM/src (ldo.cpp, lstate.cpp, lapi.cpp)
- Unity StopCoroutine and EntityCommandBuffer docs, Roblox task library docs

## State of the last edit (budget warning moved to ScriptedBodies)
Applied in the files; not built since. It removes `reportOnce` from ScriptedBehaviour and logs the budget warning from ScriptedBodies.
