# RULE: Inherit only to implement an interface — reuse code by holding an object

## Context

Pull request of the client waiting room (RTY-107). The review comment that asked for a parameter limit (`fix-architecture-at-most-three-parameters.md`) also asked to watch composition and inheritance, to avoid loops and bad practices.

## Mistake

No defect of this kind was found in the branch; the rule was asked for by the reviewer. Every inheritance the branch adds or changes is interface inheritance:
- `LobbyScreen : IScreen`;
- `ServerConnection : IServerConnection, IPlayerInputLink`;
- `RecordingServerConnection : IServerConnection`.

The waiting room holds its `Menu` as a member. The bay is free functions over `CutCornerBox` and `SunkBox`, held, not inherited.

What the audit found next to the topic:
- Four screens repeat the same members (`navigator_`, `canvas_`, `playfieldPointAt_`, `menu_`). That repetition is the usual pressure toward a base class that holds them.
- `Widgets/` and `Screens/` link each other: `rtype_client_menu_builder` (`Widgets/`) links `rtype_client_screen_interfaces` (`Screens/`), and `rtype_client_screens` links `rtype_client_widgets`. The loop dates from #69. The branch adds no edge to it, and it is reported in the pull request.

## Root cause

The project rules covered what an interface may hold (`architecture-pure-virtual-interfaces.md`), but not when to inherit at all.

## Sources read

- C++ Core Guidelines, read in `CppCoreGuidelines.md` of the isocpp/CppCoreGuidelines repository:
  - C.120, "Use class hierarchies to represent concepts with inherent hierarchical structure (only)": its reason says not to inherit when a data member will do;
  - C.121, "If a base class is used as an interface, make it a pure abstract class";
  - C.129, "When designing a class hierarchy, distinguish between implementation inheritance and interface inheritance": data in a base class makes the base harder to implement and can lead to replicated code;
  - C.133, "Avoid `protected` data";
  - C.135, "Use multiple inheritance to represent multiple distinct interfaces".
- Herb Sutter and Andrei Alexandrescu, *C++ Coding Standards*:
  - item 34, "Prefer composition to inheritance", and item 37, "Public inheritance is substitutability";
  - read through the publisher's table of contents and excerpts (https://www.oreilly.com/library/view/c-coding-standards/0321113586/ch35.html), not in full.

## Rule

1. **Public inheritance only means "can be used as".** A class derives from an interface so its callers can use it through that interface: `LobbyScreen : IScreen`, `ServerConnection : IServerConnection`.
   - Sources: C++ Core Guidelines C.121, "If a base class is used as an interface, make it a pure abstract class"; Sutter and Alexandrescu, *C++ Coding Standards*, item 37, "Public inheritance is substitutability".
   - The interface itself follows `architecture-pure-virtual-interfaces.md`.
2. **To reuse code, hold an object as a member, never inherit from it.** Example: `LobbyScreen` holds a `Menu`; it does not derive from a `MenuScreen` that holds one.
   - Why: a member can be replaced, tested, and reached through its own interface. A base class pushes its data and its construction order into every child.
   - Sources: Core Guidelines C.120, "Do *not* use inheritance when simply having a data member will do"; C.129, which separates implementation inheritance from interface inheritance; *C++ Coding Standards*, item 34, "Prefer composition to inheritance".
3. **Before writing a base class that holds data or non-virtual code for its children, write that part as a class the children hold, or as free functions they call.** A base class with data is allowed only when every child is a kind of that base in every use (C.120: "only" for concepts with an inherent hierarchy). Its doc comment says why.
4. **No `protected` data member** (C.133, "Avoid `protected` data"). A child reaches its base through functions.
5. **Several bases are several interfaces** (C.135, "Use multiple inheritance to represent multiple distinct interfaces"): `ServerConnection : IServerConnection, IPlayerInputLink`. Never two bases that both hold state.
6. **Composition points one way.**
   - Two classes never hold each other.
   - The target of a member never links back to the folder of its owner (`fix-architecture-a-link-never-points-back-at-a-folder-that-links-it.md`).
   - When two objects must talk both ways, one of them sees the other only through an interface or a callback, the way `Menu` calls the actions a screen hands it.

## Example

- ❌ **Before (wrong)**: four screens repeat `navigator_`, `canvas_`, `playfieldPointAt_` and `menu_`, so a `MenuScreen` base class takes them over and each screen derives from it. The base now decides how every screen is built and painted.
- ✅ **After (right)**: each screen derives from `IScreen` only and holds what it reuses as members. If the shared part grows, it becomes a class that each screen holds.
