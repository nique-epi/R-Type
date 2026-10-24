---
description: "No type is declared inside a class or struct: every struct, class, enum or alias a class uses has its own header"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: No type is declared inside a class — every struct, class or enum has its own header

Why: a review of the visual effects asked "why does this class have so many methods and structs" and "do not put structs in classes"; a nested type makes the class header longer and makes every caller name it `Owner::Type`.

## Rule

1. **A class or struct declares no type in its body**: no nested `struct`, `class`, `enum`, `enum class` or `union`, public or private, and no member `using` alias. Forward declarations of nested types count too.
2. **Every such type gets a header of its own, named after it** (`code-style-file-named-after-its-class.md`), with a name that makes sense without its former owner: `FlashLoan` for `FlashBudget::Loan`, `TextLine` for `TextWriter::Line`.
   - A plain struct or enum (data only, no member function) is header-only and sits in the parent folder of the class that uses it, where `project-layout.md` puts plain structs: `Effects/FlashLoan.hpp` next to `Effects/FlashBudget/`. Never inside the folder of the class, which holds that class only (`architecture-one-folder-per-concrete-class.md`).
   - A type with member functions is a concrete class: a folder of its own.
   - The dependencies a constructor takes as one struct (`fix-architecture-at-most-three-parameters.md`, point 2.2) are `<Class>Parts`, in `<Class>Parts.hpp` in the parent folder: `Screens/GameScreenParts.hpp`.
3. **A helper type that no declaration of the header names** (only the bodies of a source `.cpp` use it) goes in the anonymous namespace of that `.cpp`. When a member function's signature names it, it is part of the header: point 2 applies, or the function becomes a free function of the `.cpp`. A test `.cpp` holds no type at all: its helper types go in its fixture files (`fix-format-a-test-file-holds-only-tests.md`).
4. **Check every header the change adds or changes, before the push.** With `.clang-format` in the Google style, a namespace does not indent its content: a type at namespace scope starts at column 0, a nested one is indented.
   ```bash
   git grep -n -E '^ +(struct|class|union|enum( class)?) [A-Za-z_][A-Za-z0-9_]*( final)?( *: *[^;{]*)? *\{' -- <changed .hpp files>
   git grep -n -E '^ +(struct|class|union|enum( class)?) [A-Za-z_][A-Za-z0-9_]*;' -- <changed .hpp files>
   git grep -n -E '^ +using [A-Za-z_][A-Za-z0-9_]* *=' -- <changed .hpp files>
   ```
   - The first line finds nested definitions, the second nested forward declarations, the third member aliases. All must print nothing on the files after `clang-format`; an alias the third line finds inside the body of a template function defined in the header is not a member and is read, not counted.
   - Calibrate the first line before trusting an empty result: on `git show 'fb17f12:src/client/Screens/LobbyScreen/LobbyScreen.hpp'` it prints `struct ReadyKey` and `struct Parts`.
5. **A nested type already in a header the change touches is moved out** when its users are in the same module. A file whose only change is an `#include` line rewritten because the header it names was renamed is not touched by the change: it is reported, not reworked. When the same nesting repeats across many classes (the `Parts` of every screen, `KeySlot::Wiring`), report it and ask before moving them all (`fix-process-rework-preexisting-inconsistencies.md`). When the only folder the type can go in would make the class's library link a folder that already links it (`fix-architecture-a-link-never-points-back-at-a-folder-that-links-it.md`), it stays and is reported with that reason: `Animation::Playback` cannot go in `Sprites/`, whose target links `Sprites/Animation/`.

## Example

- ❌ **Before (wrong)**:
  ```cpp
  class FlashBudget {
   public:
    struct Loan {
      int area{};
      engine::Duration remaining{};
    };
    [[nodiscard]] bool reserve(Loan loan);
  };
  ```
  and `EffectSpawner` declaring `SheetEffect`, `DownedShip` and `Shooter` in its private part.
- ✅ **After (right)**: `src/client/Rendering/Effects/FlashLoan.hpp` holds `struct FlashLoan { int area{}; engine::Duration remaining{}; };`, and `FlashBudget/FlashBudget.hpp` declares `[[nodiscard]] bool reserve(FlashLoan loan);`; the grep of point 4 prints nothing for the two headers.
