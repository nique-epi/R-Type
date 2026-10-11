---
title: Concurrency
description: How threads hand data to each other, and where to find it in the code.
---

The concurrency library, `rtype_concurrency` (`src/concurrency/`), holds what threads use to hand data to each other. Like `rtype_logging`, it links nothing else of the repository, so the engine, the network, the client and the server may all use it; it knows nothing about R-Type, SFML or sockets. Why threads only talk through queues is explained in [Architecture](/R-Type/architecture/#queues-between-threads).

This page tells you where to look when you need to change something.

| Part | State | Where |
|---|---|---|
| Queues between threads | Available | `src/concurrency/BoundedQueue/` |

## Queues between threads: `BoundedQueue`

`BoundedQueue<Message>` (`src/concurrency/BoundedQueue/`) hands messages from one thread to another, for example the datagrams the network thread receives to the thread that runs the game. Every method may be called from any thread. None of them waits for a message to arrive; a call only waits while another thread holds the queue's lock for one push or one drain.

- `push(message)` adds the message after the others. The room, given to the constructor, is reserved once; when the queue is full, its oldest message is discarded to make room, and `discardedCount()` counts it. A room of 0 throws `InvalidQueueCapacityException`.
- `drainInto(destination)` moves every waiting message, oldest first, to the end of a `std::vector`, and leaves the queue empty. It makes room in the vector before moving anything, so when memory runs out the queue is left as it was. With nothing waiting it returns at once, so a game loop drains once per tick and carries on whatever arrived. Clearing and reusing the same vector every tick avoids allocating it again.
- A message must satisfy `QueueableMessage`: it is default-constructible, and moving it never throws. A queue of another type does not compile. The queue moves messages in and out and never copies them, so a message can own its bytes.

## Where to intervene

| You want to… | Look at |
|---|---|
| Hand data from one thread to another | `BoundedQueue` in `src/concurrency/BoundedQueue/` |
| Add a concurrency error | `src/concurrency/Exceptions/ConcurrencyException.hpp` |
| Test code that runs on two threads | `tests/concurrency/BoundedQueueTest.cpp`, built into `concurrency_tests`, which stops a hanging test after 60 seconds |

Each folder has its own `CMakeLists.txt` declaring one library, linked into `rtype_concurrency` by `src/concurrency/CMakeLists.txt`.
