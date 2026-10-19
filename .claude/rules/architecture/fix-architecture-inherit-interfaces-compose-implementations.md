---
description: "Inherit only to implement an interface; reuse code by holding an object as a member, never by inheriting it"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: Inherit only to implement an interface — reuse code by holding an object

Why: a review asked to watch composition and inheritance. Inheritance couples a class to its base more tightly than anything but friendship, and a base class added to share code is hard to remove later.

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
