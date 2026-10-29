---
description: "A class declares at most ten member functions besides its constructors, destructor and defaulted or deleted members; one that would pass it is split"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A class declares at most ten member functions — a class that would pass it is split

Why: a review asked "why does this class have so many methods and structs" about `EffectSpawner`, 33 member functions, and to watch the length of classes; PMD's `TooManyMethods` reports a class above 10 by default, and 90 % of the classes on `origin/main` have 9 or fewer.

## Rule

1. **A class or struct declares at most 10 member functions**, public, protected and private together, static included, a member template counted once. Not counted: constructors, the destructor, and members declared `= default` or `= delete`. Test fixtures and test doubles count like any class.
2. **A class the change adds stays at 10 or fewer. A class the change gives new member functions ends at 10 or fewer**, even when the class was under the limit before and the change is what pushes it over.
3. **When a class would pass the limit, split it along the state its functions touch**, never by cutting it into arbitrary halves:
   - list, for each member function, the members it reads or writes; functions that share a state form a class that holds that state (`FlashBudget`, `KillChain` were born this way);
   - functions that only read the registries become free functions in a pair of their own (`architecture-a-module-is-one-folder-and-one-target.md`), taking what they read as parameters;
   - the remaining class holds the new ones as members and calls them (`fix-architecture-inherit-interfaces-compose-implementations.md`); its public interface does not grow to forward theirs.
   - Grouping is by meaning: the parameter rule's point 5 holds here too, no `…Helper` class holding whatever was left over.
4. **Measure every class the change adds or changes, before the push**, with `clang-query` on the `.cpp` of the class (the header alone has no compile command), with the SDK flags of `fix-execution-run-clang-tidy-before-push.md`:
   ```bash
   class=EffectSpawner
   member="anyOf(hasParent(cxxRecordDecl(hasName(\"$class\"), isDefinition())), hasParent(functionTemplateDecl(hasParent(cxxRecordDecl(hasName(\"$class\"), isDefinition())))))"
   clang-query -p build <SDK flags> -c 'set output diag' \
     -c "match cxxMethodDecl($member, unless(anyOf(cxxConstructorDecl(), cxxDestructorDecl(), isDefaulted(), isDeleted(), isImplicit(), isTemplateInstantiation())))" \
     <the .cpp of the class>
   ```
   - The last line, `N matches.`, is the count. It must be 10 or fewer.
   - Calibration at commit `275b065`: `EffectSpawner` 33, `TextWriter` 5, `ServerConnection` 36 (one member template, counted once), the same counts as a hand count of the headers. Without the `functionTemplateDecl` branch the template is missed (35); without `isTemplateInstantiation` its instantiations are counted (40).
   - A class with no `.cpp` (a fixture or a struct in a header) is counted by reading its body against point 1.
5. **A class over the limit that the change does not give a member function is reported, not split** (`fix-process-rework-preexisting-inconsistencies.md`). Measured on `origin/main` at `56a7019`: `GameInstance` 31, `OptionsScreen` 31, `MenuBuilder` 27, `Menu` 26, `Button` 20, `Dropdown` 20, `Slider` 16, `ScreenStack` 15, among others.

## Example

- ❌ **Before (wrong)**: `EffectSpawner` with 33 member functions and nine members: the flash preference and budget, the kill chain, the gains, the camera shake, the pixel effects, the registries and the time since the start.
- ✅ **After (right)**: the flash gate, the sheet effects, the gains and the lookups in the registries are classes or free functions of their own; `EffectSpawner` holds them and plays the ladder, and the query of point 4 prints 10 matches or fewer for each class.
