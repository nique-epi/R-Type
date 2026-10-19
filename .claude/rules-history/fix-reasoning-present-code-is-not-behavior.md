# RULE: Present code is not behavior — require execution evidence

The rule was imported from another project when R-Type started; its original incident belongs to that repository. This file holds the incidents met on R-Type.

## Update (2026-10-19) — a send described without its recipients

### Context
The user asked why the waiting room never leaves for the game once every player is ready, then asked to verify the answer.

### Mistake
The answer said the server sends its `WorldState` during a game "without the packet header", taken from `server.md` and from `WorldStateSender::onTick`, which encodes the bare payload. The call site in `src/server/main.cpp` passes `recipients`, a `const` empty vector nothing fills, and `onTick` returns at once on an empty list: no state is ever sent. `server.md` said so in its first paragraph and its state table. The same sentence went into `client.md`.

### Root cause
The shape of the datagram was read in the sender, and its sending was assumed; the call site and its list of recipients were not read.

### Rule
1. Before saying a program sends a message, read the call site of the code that sends it and the list of recipients it is given there. A sender handed an empty list, or a list nothing fills, sends nothing.
2. A sentence about the shape of what is sent ("without a header", "every 50 ms") is never a proof that it is sent: check the sending happens first, then describe its shape.

### Example
- ❌ **Before (wrong)**: "the server sends its world states during a game, but without the packet header".
- ✅ **After (right)**: `main.cpp` passes an empty `recipients` to `WorldStateSender::onTick`, which returns on an empty list → "the server sends no world state at all; once it does, the datagrams will also need the header".
