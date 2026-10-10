---
title: Server
description: What the server does, how its threads fit together and where to find them in the code.
---

The server, `r-type_server`, runs the game for the players connected to it: it receives their datagrams and advances the game at a fixed rate; sending the state back to them is not written yet. It never links SFML or Asio: it reaches the network through `rtype_network`, and its own libraries are checked for it at configure time (see [Project layout](/R-Type/project-layout/#who-may-link-what)).

This page tells you where to look when you need to change something. It describes what exists today and says plainly what does not exist yet.

| Part | State | Where |
|---|---|---|
| Network thread and simulation thread | Available | `src/server/Application/` |
| Timer resolution on Windows | Available | `src/server/Platform/` |
| Filtering unwanted senders before the incoming queue | Not yet written | — |
| Launch options, lobby, games, replication, master link | Not yet written | — |

## Threads: `ServerApplication`

`ServerApplication` (`src/server/Application/ServerApplication/`) runs the server on two threads, which never share game state:

- The **network thread**, the one that calls `run()`, runs the `NetworkContext`. `DatagramRelay` copies every datagram received, with its sender, into the incoming `BoundedQueue`, and sends what was left in the outgoing one. It is the only thread that touches the socket.
- The **simulation thread** runs `SIMULATION_TICKS_PER_SECOND` (60) ticks per second. Each tick drains the incoming queue and hands what arrived, often nothing, to the tick handler given to the constructor, so a slow or silent player never holds the game up. At the end of the tick it asks the network thread, through `NetworkContext::post()`, to send the outgoing queue.

The tick handler is where the game plugs in. Today `src/server/main.cpp` gives one that logs each datagram at the `debug` level: nothing decodes or answers them yet, and nothing fills the outgoing queue.

The queues hold `INCOMING_DATAGRAM_QUEUE_CAPACITY` and `OUTGOING_DATAGRAM_QUEUE_CAPACITY` (256) datagrams each (`src/server/ServerConstants.hpp`). These sizes are **provisional**: about one second of the inputs of four players, they will be revised with the rates of the protocol. Code must read the constants.

- When more datagrams arrive between two ticks than the incoming queue holds, the oldest are discarded. Every `TICKS_PER_REPORT` ticks (600, so every ten seconds), the server logs a warning with how many were discarded, if any.
- Nothing filters the senders yet: one sender flooding the port pushes the other senders' datagrams out of the incoming queue.
- The outgoing queue also discards its oldest datagram when full, but nothing reports it yet.

`stop()` makes `run()` return, from any thread: the network thread stops at once, the simulation thread when its current sleep ends, about one tick later. An exception thrown on either thread, by the tick handler for instance, stops both, and `run()` rethrows it.

## Tick loop: `TickLoop`

`TickLoop` (`src/server/Application/TickLoop/`) calls a function once per tick on the thread that runs it. It counts ticks with a `FixedTimestep` and sleeps until its `nextTickTime()`, so the average rate stays at 60 ticks per second: after a late wake-up the missed ticks run back to back, and a stall longer than `MAXIMUM_TICKS_PER_ADVANCE` (5) ticks drops the excess. `ServerApplication` creates it when `run()` starts, so the first tick is due one tick after the call.

Every `TICKS_PER_REPORT` ticks, the loop logs at the `debug` level how late its ticks started, each compared with the moment it was due; a dropped stall shows in full. The journal is `r-type_server.log`; `--log-stderr` also prints it in the terminal (see [Logging](/R-Type/logging/#choosing-the-level-and-the-output-at-launch)):

```bash
./r-type_server --log-level debug --log-stderr
```

After the time stamp, the line reads:

```
[DEBUG] [Simulation] - 600 ticks started on average 4.43782 ms late, at most 8.32842 ms
```

How late a tick starts depends on how precisely the system wakes a sleeping thread:

- On Windows, MSVC's `sleep_for` ends in `Sleep`, whose documentation says that a wait longer than one tick of the system clock but shorter than two "can be anywhere between one and two ticks". While it runs, the loop holds a `FineTimerResolution` (`src/server/Platform/`), which asks Windows for `FINE_TIMER_RESOLUTION_MILLISECONDS` (1 ms). If Windows refuses, the loop logs a warning and runs anyway.
- On macOS, measured on an Apple Silicon laptop, a sleep wakes later the longer it is: 0.5 ms late on average for a 1 ms sleep, 5.6 ms for a 16.7 ms sleep. Timer coalescing, enabled on that machine, is the likely cause. Ticks start 4.4 ms late on average and at most 8.3 ms late, with no client as with four clients sending 60 datagrams per second each, and 600 ticks still run every ten seconds.
- On Linux and Windows, the lateness is not measured yet.

## Where to intervene

| You want to… | Look at |
|---|---|
| Change what a tick does | The tick handler given to `ServerApplication` in `src/server/main.cpp` |
| Change the size of the queues between threads | `INCOMING_DATAGRAM_QUEUE_CAPACITY` and `OUTGOING_DATAGRAM_QUEUE_CAPACITY` in `src/server/ServerConstants.hpp` |
| Change the simulation rate | `SIMULATION_TICKS_PER_SECOND` in `src/engine/Time/TimeConstants.hpp` |
| Change how often the simulation reports | `REPORT_SECONDS` in `src/server/ServerConstants.hpp` |
| Write code that differs between Windows and the other systems | `src/server/Platform/`: a `Windows.cpp` file and a `Posix.cpp` file per class, chosen in its `CMakeLists.txt` |
| Test code that uses a socket without the network | `tests/doubles/RecordingDatagramSocket.hpp` |
| Test the whole server | `tests/server/Application/ServerApplicationTest.cpp`: a client on the loopback address, and a tick handler that records or throws |

Each folder holding sources has its own `CMakeLists.txt` declaring one library; `Application/` and `Platform/` only add their subfolders, and `r-type_server` links `rtype_server_application` in `src/server/CMakeLists.txt`.
