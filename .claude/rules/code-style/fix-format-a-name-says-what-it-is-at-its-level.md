---
description: A name says what the thing is or does, at its real level of generality; a generic name only for a generic system, the domain in the name otherwise
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A name says what the thing is, at its real level of generality

Why: a reader must know what a class, a function or a variable does from its name alone, and a misleading name costs more than a meaningless one (Avidan and Feitelson, ICPC 2017).

## Rule

1. **Build the name from what the thing is, in three steps** (Feitelson et al., "How Developers Choose Names", 2022): list the notions a reader needs (what it holds or does, for whom, when), pick one word per notion, then join them. A name that leaves out a notion the reader needs is incomplete; a word that adds nothing goes.
2. **A generic name only for a generic system.** A type that works for any case of its kind takes the plain word (`ScriptMachine` only if it runs any script language). A type tied to one domain, one library or one language carries it in its name: `LuauThread`, not `ScriptThread`, for a thread only Luau runs.
3. **Name the role, never the type or the mechanism** (Swift API Design Guidelines, "name according to roles"; C++ Core Guidelines NL.5): `HeldIntent` for the intent a wait holds, not `Routine` for a class that runs nothing; `IBehaviourState` for what a behaviour keeps of an entity, not `IBehaviourMemory`.
4. **Use the established word of the domain or of the library**, as their documentation writes it: Luau's own `thread`, `resume`, `yield`, `protected call`; the ECS's `entity`. A project word (`body`) is used only where it means exactly that, never for the general case.
5. **The broader the scope, the more context the name carries** (Google C++ Style, "Choosing Names"; C++ Core Guidelines ES.7): a class or a header name stands alone across the repository; a local in five lines may be short.
6. **One name, one meaning in a scope**: two things called `state` in one function (a `lua_State` and the state of a behaviour) are renamed until each name names one thing.
7. **No filler word** (`Manager`, `Helper`, `Data`, `Info`, `Utils`, `Handler`): see `fix-format-name-after-data-not-process.md`.

### Check before the push

For each type, function and member the change adds or renames, write in one line what it is, then compare the line with the name. A name a reader would expand into another sentence is renamed before the push, and a pull request lists the renames with that line.

## Example

- ❌ **Before (wrong)**: `Routine`, a class that only counts the ticks of a wait and holds the intent of that wait; `ScriptThread`, `ScriptStepEnd`, `ScriptProtection`, only ever about Luau; `IBodyOrders` for orders that create any entity; `state` for a `LuauBehaviourState` beside `machine.state()`.
- ✅ **After (right)**: `HeldIntent`; `LuauThread`, `LuauResumeResult` (`Returned`, `Yielded`), `LuauProtectedCall`; `IEntityOrders`; `behaviourState`.
