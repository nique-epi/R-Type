---
description: C++ interfaces — only pure virtual methods (= 0) and a = default virtual destructor
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: An interface only exposes pure virtual methods

## Goal

An interface describes a **contract**, not a behavior. Giving it a default implementation (`virtual void update() {}`) blurs the line between interface and abstract class, and hides missing implementations in derived classes. This matters all the more on R-Type since the engine boundaries (rendering, audio, network, input) go through interfaces.

## Rules

- An **interface** (a class only meant to be implemented, prefixed with `I`: `IRenderer`, `ISystem`, `INetworkClient`) only has:
  - **pure virtual** methods (`= 0`);
  - a **`= default` virtual destructor**, the only tolerated "implementation".
- **No interface method has a `{}` body**, not even an empty one.
- No data member in an interface.
- A **partially** implemented abstract class (shared state, common methods) is not an interface: it does not carry the `I` prefix and is not covered by this rule.

## Examples

- ❌ **Forbidden**:
  ```cpp
  class ISystem {
   public:
    virtual ~ISystem() = default;
    virtual void update(float deltaTime) {}
    virtual void reset() {}
  };
  ```
- ✅ **Instead**:
  ```cpp
  // ISystem.hpp
  class ISystem {
   public:
    virtual ~ISystem() = default;
    virtual void update(float deltaTime) = 0;
    virtual void reset() = 0;
  };
  ```
