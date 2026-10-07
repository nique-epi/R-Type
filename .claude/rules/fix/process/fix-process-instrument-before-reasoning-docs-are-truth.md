---
description: Instrument both ends of a channel before reasoning; an API behavior is only asserted after reading the docs or headers
trigger: always_on
---

# RULE: Instrument BOTH ends of a channel before reasoning, and conclude only from the docs — never from an API hunch

## Rule

### 1. Instrument both ends of a channel, before the first line of fix

As soon as a value crosses a boundary — client → network → server, server → snapshot → client, key press → component → system — and the observed behavior does not match, **put a log where it is emitted and a log where it is received**, with the identity of the sender and the receiver (client id, sequence number, entity id, tick). A send without a matching receive *is* the answer.

### 2. Two round trips without evidence = stop coding

At the **2nd** user feedback on the same symptom, no further fix goes out until a measurement has been produced: instrument, have it run, read. This is the "what to do instead" of `fix-process-do-not-over-engineer-step-back.md`.

### 3. The source of truth is the documentation, not intuition

No API behavior is asserted, coded, or written in a doc comment without having been **read**, in this order:

1. the **headers of the installed version** (`build/vcpkg_installed/<triplet>/include/` after a configure) — they describe the version actually compiled (SFML 3 differs a lot from SFML 2);
2. the **official documentation** of that version;
3. a **web search**, as soon as the topic is a subtle behavior (lifetime of a buffer passed to `async_receive_from`, which thread runs a handler, order of SFML events).

Cite the source in the answer. A claim without a source is a hypothesis and is stated as such.

### 4. A doc comment only contains verified facts

An assumption written in a comment becomes a fact for every following session.

### 5. Prefer the API that makes the bug impossible

When two primitives exist, choose the one whose **shape** rules out the class of bug. A failed lookup returns an empty `std::optional` or an `end()` iterator, never a plausible neighbor (index 0, default entity).

### 6. A guarantee taken from a source keeps the source's scope

1. **Copy the qualifiers with the claim**: "expected", "on modern IPv4 paths", "for compliant implementations", "by default". A source that says "should" or "is expected to" never becomes "always" or "never" in my text.
2. **Before writing "never", "always", "on any path", "in every case"**, find the sentence of the source that says it. If none does, the word goes, and the case the source leaves open is written down ("a path with a smaller MTU may still fragment it").
3. **Reread the cited section for its limits**, not only for the figure I came for: the exceptions are usually in the next paragraph.

## Example

- ❌ **Before (wrong)**: "the client doesn't see the other players, it must be the interpolation" → three fixes to the interpolation.
- ✅ **After (right)**: a log when the snapshot is sent (tick, entity count) and one when it is received (tick, entity count) → the client receives 0 entities → the server serializes before adding the players → targeted fix.
- ❌ **Before (wrong)**: RFC 9000 says 1200-byte datagrams are "expected" to pass on "modern IPv4 and all IPv6" paths → the page says "IP never fragments a datagram of this protocol, on any path".
- ✅ **After (right)**: "QUIC relies on the same figure and expects it to pass on all IPv6 paths and on modern IPv4 paths; a path with a smaller MTU may still fragment or drop one of this protocol's datagrams." (RFC 9000's ban on IP fragmentation binds QUIC senders, not this protocol.)
