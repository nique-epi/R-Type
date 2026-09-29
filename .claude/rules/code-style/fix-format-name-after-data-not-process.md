---
description: Structural classes and files are named after the data or domain they serve, never after a process or jargon
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: Structural artifacts are named after the DATA / DOMAIN they serve, never after a process or jargon

## Rule

1. **A type that holds or stores data is named after that data**: `EntityRegistry`, `PlayerInput`, `Snapshot`, `ClientSession`. Never after the process that uses it (`SyncData`, `NetworkStuff`) nor after its consumer.
2. **Process names are for process artifacts**: a system that applies movement can be called `MovementSystem`, a type that serializes `SnapshotSerializer` — it *is* the process. It consumes `Snapshot`; it is not called `SnapshotData`.
3. **No catch-all suffix** (`Manager`, `Handler`, `Helper`, `Utils`, `Data`, `Info`) when a precise term exists: `ClientRegistry` rather than `ClientManager`, `PacketDispatcher` rather than `PacketHandler` if it routes to handlers.
4. **No prefix that repeats the namespace or folder**: `rtype::network::Packet`, not `rtype::network::NetworkPacket`.
5. Test: a teammate discovering the repository understands what the file contains from its name alone.

## Example

- ❌ **Before (wrong)**: `NetworkManager.hpp` holding the socket, the client list and the serialization; `GameData.hpp` for the state of a game.
- ✅ **After (right)**: `UdpServer.hpp` (socket), `ClientRegistry.hpp` (clients), `SnapshotSerializer.hpp` (serialization), `GameState.hpp` (game state).
