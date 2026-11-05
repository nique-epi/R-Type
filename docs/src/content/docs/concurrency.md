---
title: Concurrency
description: How threads hand data to each other, and where to find it in the code.
---

The concurrency library, `rtype_concurrency` (`src/concurrency/`), holds what threads use to hand data to each other. Like `rtype_logging`, it links nothing else of the repository, so the engine, the network, the client and the server may all use it; it knows nothing about R-Type, SFML or sockets. Why threads only talk through queues is explained in [Architecture](/R-Type/architecture/#queues-between-threads).

This page tells you where to look when you need to change something.

| Part | State | Where |
|---|---|---|
| Queues between threads | Available | `src/concurrency/BoundedQueue/` |
| The newest value of a thread | Available | `src/concurrency/LatestValue/` |
| A sleep a stop request ends | Available | `src/concurrency/InterruptibleSleep/` |

## Queues between threads: `BoundedQueue`

`BoundedQueue<Message>` (`src/concurrency/BoundedQueue/`) hands messages from one thread to another, for example the datagrams the network thread receives to the thread that runs the game. Every method may be called from any thread. None of them waits for a message to arrive; a call only waits while another thread holds the queue's lock for one push or one drain.

- `push(message)` adds the message after the others. The room, given to the constructor, is reserved once; when the queue is full, its oldest message is discarded to make room, `discardedCount()` counts it, and `push()` returns `true`, so the caller knows that its message made the queue overflow. A room of 0 throws `InvalidQueueCapacityException`.
- `drainInto(destination)` moves every waiting message, oldest first, to the end of a `std::vector`, and leaves the queue empty. It makes room in the vector before moving anything, so when memory runs out the queue is left as it was. With nothing waiting it returns at once, so a game loop drains once per tick and carries on whatever arrived. Clearing and reusing the same vector every tick avoids allocating it again.
- A message must satisfy `QueueableMessage`: it is default-constructible, and moving it never throws. A queue of another type does not compile. The queue moves messages in and out and never copies them, so a message can own its bytes.

## The newest value of a thread: `LatestValue`

`LatestValue<Value>` (`src/concurrency/LatestValue/`) keeps the newest value one thread publishes for others to read, as a watch channel does: a reader gets the latest value, never a queue of them. The server keeps its newest metrics report in one (`LatestMetrics`), and the list of its games in another (`LatestHostedGames`), which MasterLink, the thread of the master module, waits on.

- `publish(value)` replaces the value and wakes every reader that waits. `revision()` counts the values published, 0 before the first one.
- `value()` gives a copy of the newest value, or nothing before the first one. Reading changes nothing, so readers at different rates all see the same value.
- `waitForNewer(seen, timeout, stop)` sleeps until a value newer than revision `seen` comes, `timeout` passed or `stop` is requested, and returns the revision it wakes with.
- The lock is held for a move or a copy only, so a slow reader never slows the thread that publishes. Use it for a state that only its newest version matters for; a `BoundedQueue` for messages that must all arrive.

## A sleep a stop request ends: `sleepUnlessStopped`

`sleepUnlessStopped(stop, duration)` (`src/concurrency/InterruptibleSleep/`) sleeps `duration` on the calling thread, or less once `stop` is requested, before the call or during it. `std::this_thread::sleep_for` cannot be woken, so a thread that waits seconds between two tries would keep the program from stopping that long; this one waits on a `std::condition_variable_any` that nothing notifies, whose `wait_for` wakes when the stop is requested. MasterLink sleeps with it between two registrations and between two heartbeats. A thread that only sleeps a few milliseconds, such as the storage thread, keeps `sleep_for`.

## Where to intervene

| You want to… | Look at |
|---|---|
| Hand data from one thread to another | `BoundedQueue` in `src/concurrency/BoundedQueue/` |
| Share the newest state of a thread, or wait for a newer one | `LatestValue` in `src/concurrency/LatestValue/` |
| Sleep in a thread without delaying its stop | `sleepUnlessStopped` in `src/concurrency/InterruptibleSleep/` |
| Add a concurrency error | `src/concurrency/Exceptions/ConcurrencyException.hpp` |
| Test code that runs on two threads | `tests/concurrency/BoundedQueueTest.cpp`, `tests/concurrency/InterruptibleSleepTest.cpp` and `tests/concurrency/LatestValueTest.cpp`, built into `concurrency_tests`, which stops a hanging test after 60 seconds |

Each folder has its own `CMakeLists.txt` declaring one library, linked into `rtype_concurrency` by `src/concurrency/CMakeLists.txt`.
