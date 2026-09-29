---
description: Identifiers — full, explicit names, never abbreviations, even idiomatic ones
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: No abbreviations in identifiers

## Goal

An abbreviation that is idiomatic to someone who knows the domain is opaque to every other reader — a teammate discovering the network module, the Epitech jury. Code is read far more often than written: the cost when writing is negligible, the readability gain lasts.

## Rules

- Methods, functions, classes, members and variables use **full, explicit names**, never abbreviations — even when the abbreviation is common in the domain (`pos`, `vel`, `dt`, `ent`, `cmp`, `sys`, `pkt`, `buf`, `cfg`, `nb`, `idx`, `mgr`).
- Only exceptions:
  - names from the standard library and dependencies (`std::size_t`, `asio::ip::udp`);
  - acronyms that became words (`id`, `udp`, `tcp`, `ecs`, `ui`), written as words: `clientId`, `udpSocket`;
  - trivial loop counters over an index (`i`) when the loop fits in a few lines.
- Applies to every new declaration and every declaration touched during a refactoring.

## Examples

- ❌ **Forbidden**:
  ```cpp
  void updatePos(Entity ent, float dt);
  std::size_t nbPkts;
  PacketMgr pktMgr;
  ```
- ✅ **Instead**:
  ```cpp
  void updatePosition(Entity entity, float deltaTime);
  std::size_t packetCount;
  PacketDispatcher packetDispatcher;
  ```
