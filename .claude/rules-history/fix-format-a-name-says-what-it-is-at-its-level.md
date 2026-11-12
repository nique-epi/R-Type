# RULE: A name says what the thing is, at its real level of generality

## Context

The coroutine work of the scripted enemies (PR #309) added a scripting stack whose names said little: `Routine`, `IBehaviourMemory`, `LuauBodyMemory`, `ScriptThread`, `ScriptStepEnd`, `ScriptProtection`, `LuauStepState`, `LuauView`, `IScriptWorld`, `BodyOrders`.

## Mistake

Nolan could not tell from the names what the classes did. `Routine` read as a homemade coroutine, while it only held the intent of a wait; engine types named `Script…` ran Luau only; "world" named orders to create bodies, next to `game::World`; "body" was used where the scripts were about to reach every entity; a function held two unrelated things named `state`.

## Root cause

Names were chosen from the mechanism or the first word at hand, never derived from what the thing is, and no rule asked whether a name was as generic as its system.

## Sources read

- Swift API Design Guidelines: clarity at the point of use, names by role, include the words needed, omit needless words, embrace precedent.
- Microsoft Framework Design Guidelines, General Naming Conventions: semantically interesting names, generic names only without semantic meaning.
- Google C++ Style Guide, Choosing Names; C++ Core Guidelines ES.7 and NL.5.
- Domain-Driven Design, ubiquitous language.
- Feitelson et al., "How Developers Choose Names" (TSE 2022): a three-step model, names built with it judged better about two to one.
- Avidan and Feitelson, ICPC 2017: misleading names hurt comprehension, a small study (9 developers, 6 methods).

## Rule and example

See `.claude/rules/code-style/fix-format-a-name-says-what-it-is-at-its-level.md`.
