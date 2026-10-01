---
description: File name = exact PascalCase name of the main class it declares or defines
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: File name = `PascalCase` name of the main class

## Goal

The file name must be mechanically derivable from the class name, and vice versa: you look for `PacketReader`, you open `PacketReader.hpp`. A `packet_reader.hpp` in the middle breaks consistency and makes navigation harder.

## Rules

- A `.hpp` / `.cpp` carries the **exact `PascalCase` name of the main class (or entity)** it declares/defines. No `snake_case`, no `lowercase`, no `kebab-case`.
- The `I` prefix of interfaces is part of the name: `ISystem.hpp`.
- A file with no class (free functions, constants) takes the `PascalCase` name of the concept it groups: `NetworkConstants.hpp`.
- A test file is named after what it tests, with a `Test` suffix: `PacketReaderTest.cpp`.
- Exception: `main.cpp` entry points.
- An existing file that breaks the convention is renamed as soon as it is touched.

## Examples

- ❌ **Forbidden**:
  ```
  packet_reader.hpp        // class PacketReader
  entityregistry.cpp       // class EntityRegistry
  udp-server.hpp           // class UdpServer
  ```
- ✅ **Instead**:
  ```
  PacketReader.hpp
  EntityRegistry.cpp
  UdpServer.hpp
  ```
