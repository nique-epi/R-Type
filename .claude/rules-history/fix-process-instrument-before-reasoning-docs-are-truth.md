# History: fix-process-instrument-before-reasoning-docs-are-truth

The rule was imported from another project when R-Type started; its original incident belongs there. This file records the updates made on R-Type.

## Update (2026-10-07) — A guarantee taken from a source keeps the source's scope

### Context

Writing version 0 of the network protocol page. The maximum datagram size (1200 bytes of UDP payload) was justified with RFC 8200 (IPv6 minimum MTU of 1280 bytes) and RFC 9000, section 14 (QUIC uses the same figure). Both sections were read in their raw text.

### Mistake

The page concluded: "So IP never fragments a datagram of this protocol, on any path." RFC 9000 itself only says that "modern IPv4 and all IPv6 network paths are expected to be able to support QUIC". An IPv4 path or a tunnel with a smaller MTU can still fragment or drop the datagram. The pull request review flagged the overstatement.

### Root cause

I read the section for its figure and kept the figure, then turned the source's "expected" on a named set of paths into "never" on every path. The qualifier was in the very paragraph I quoted.

### Rule

Copy the qualifiers with the claim; before writing "never", "always" or "on any path", find the sentence of the source that says it, or drop the word and write down the case the source leaves open; reread the cited section for its limits.

### Example

- ❌ **Before (wrong)**: "IP never fragments a datagram of this protocol, on any path."
- ✅ **After (right)**: "QUIC relies on the same figure and expects it to pass on all IPv6 paths and on modern IPv4 paths; a path with a smaller MTU may still fragment or drop a datagram."

## Update (2026-10-24) — a template constructor read from its declaration only

**Context.** The music player of the client (RTY-123) loops its theme from its first beat, a constant in `std::chrono::milliseconds` stored as the `engine::Duration` of the project (nanoseconds), handed to `sf::Time`.

**Mistake.** `sf::Time(THEME_FIRST_BEAT)` was written after checking that `Time.hpp:57` declares a constructor template from any `std::chrono::duration`. The build failed: the definition, `Time.inl:39`, initializes a `std::chrono::microseconds` member from the duration, an implicit conversion that refuses nanoseconds.

**Root cause.** The header was read for the existence of the constructor, not for what its body converts; a template's declaration accepts every type its body may still refuse.

**Addition to the rule.** A template constructor or function of a library is read with its definition before a type of my choice is passed to it; a value goes in as the type the definition stores.

## Update (2026-11-04) — a delay named from the first timeout found

**Context.** A review of the load-test bots (RTY-98) asked to qualify `PlayerConnection::leave()`: the server frees the seat of a player who leaves as soon as one copy of its `Disconnection` arrives, and otherwise later.

**Mistake.** The new doc comment, and the same sentence in `ClientConnectionModule.hpp`, said the server frees the place "once the player was silent for SESSION_TIMEOUT". A second review found that at `SESSION_TIMEOUT` the server only marks the player away and keeps its seat for `RECONNECTION_GRACE`; the session closes, and the seat is freed, after `SESSION_CLOSING_TICKS` ticks of silence (`SESSION_SILENCE_LIMIT`, 20 s while the server keeps its pace). A third review added that last condition: the server counts the ticks it runs, so a stall makes the real delay longer.

**Root cause.** The delay was taken from `server.md` ("timed out: nothing received … for 5 s") and from the name of the constant, not from the line of `Departures.cpp` that closes the session; the doc comment of `RECONNECTION_GRACE`, two constants further in the same header, said it is counted from `SESSION_TIMEOUT`.

**Addition to the rule.** A delay written next to an effect is the one the code compares where the effect happens: find that line, copy its constant, and read the doc comments of the constants around it.
