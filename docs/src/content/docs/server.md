---
title: Server
description: What the server does, how its threads fit together and where to find them in the code.
---

The server, `r-type_server`, runs the game for the players connected to it: it receives their datagrams, advances the game at a fixed rate and sends the result back. It never links SFML or Asio: it reaches the network through `rtype_network`, and its own libraries are checked for it at configure time (see [Project layout](/R-Type/project-layout/#server)).

This page tells you where to look when you need to change something. It describes what exists today and says plainly what does not exist yet.

| Part | State | Where |
|---|---|---|
| Network thread and simulation thread | Available | `src/server/Application/` |
| Timer resolution on Windows | Available | `src/server/Platform/` |
| Launch options, lobby, games, replication, master link | Not yet written | — |

## Threads: `ServerApplication`

`ServerApplication` (`src/server/Application/ServerApplication/`) runs the server on two threads, which never share game state:

- The **network thread**, the one that calls `run()`, runs the `NetworkContext`. `DatagramRelay` copies every datagram received, with its sender, into the incoming `BoundedQueue`, and sends what the simulation left in the outgoing one. It is the only thread that touches the socket.
- The **simulation thread** runs `SIMULATION_TICKS_PER_SECOND` (60) ticks per second. Each tick drains the incoming queue and carries on with whatever arrived, so a slow or silent player never holds the game up. At the end of the tick it asks the network thread, through `NetworkContext::post()`, to send the outgoing queue.

The queues hold `INCOMING_DATAGRAM_QUEUE_CAPACITY` and `OUTGOING_DATAGRAM_QUEUE_CAPACITY` (256) datagrams each (`src/server/ServerConstants.hpp`). When the incoming queue overflows, its oldest datagrams are discarded, and the next tick logs a warning with their number. These sizes are **provisional**: about one second of the inputs of four players, they will be revised with the rates of the protocol. Code must read the constants.

Today a tick only logs each datagram at the `debug` level: nothing decodes or answers them yet, and the `World` has nothing to advance.

`stop()` makes `run()` return, from any thread: the network thread stops at once, the simulation thread when its current sleep ends, about one tick later. An exception on either thread stops both, and `run()` rethrows it.

## Tick loop: `TickLoop`

`TickLoop` (`src/server/Application/TickLoop/`) calls a function once per tick on the thread that runs it. It counts ticks with a `FixedTimestep` and sleeps until the next one is due, so the rate is exact over time: after a late wake-up the missed ticks run back to back, and a stall longer than `MAXIMUM_TICKS_PER_ADVANCE` (5) ticks drops the excess.

Every `TICKS_PER_LATENESS_REPORT` ticks (600, so every ten seconds), the loop logs at the `debug` level how late its ticks started, each compared with the moment it was due. Launch the server with `--log-level debug` to read it:

```
[Simulation] - 600 ticks started on average 4.43782 ms late, at most 8.32842 ms
```

How late a tick starts depends on how precisely the system wakes a sleeping thread:

- On Windows, a sleeping thread only wakes on a tick of the system clock: according to the documentation of `Sleep`, a sleep longer than one clock tick but shorter than two lasts anywhere between one and two of them, and MSVC's `sleep_until` calls `Sleep`. While it runs, the loop holds a `FineTimerResolution` (`src/server/Platform/`), which asks Windows for clock ticks of `FINE_TIMER_RESOLUTION_MILLISECONDS` (1 ms).
- On macOS, a wake-up is delayed in proportion to the length of the sleep (timer coalescing). Measured on an Apple Silicon laptop, ticks start 4.4 ms late on average and at most 8.3 ms late, with no client as with four clients sending 60 datagrams per second each; 600 ticks still run every ten seconds.
- On Linux and Windows, the lateness is not measured yet.

## Where to intervene

| You want to… | Look at |
|---|---|
| Change what a tick does | `ServerApplication::simulateTick()` in `src/server/Application/ServerApplication/` |
| Send a datagram from the simulation | In `simulateTick()`, push an `OutgoingDatagram` to the outgoing queue: the network thread sends it at the end of the tick |
| Change the size of the queues between threads | `INCOMING_DATAGRAM_QUEUE_CAPACITY` and `OUTGOING_DATAGRAM_QUEUE_CAPACITY` in `src/server/ServerConstants.hpp` |
| Change the simulation rate | `SIMULATION_TICKS_PER_SECOND` in `src/engine/Time/TimeConstants.hpp` |
| Change how often tick lateness is logged | `LATENESS_REPORT_SECONDS` in `src/server/ServerConstants.hpp` |
| Write code that differs between Windows, Linux and macOS | `src/server/Platform/`: one `.cpp` per system, chosen in its `CMakeLists.txt` |
| Test code that uses a socket without the network | `tests/doubles/RecordingDatagramSocket.hpp` |

Each folder has its own `CMakeLists.txt` declaring one library; `r-type_server` links `rtype_server_application` in `src/server/CMakeLists.txt`.
