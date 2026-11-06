---
title: Network measurements
description: What one client receives and sends per second, with and without each optimization of the network, how it was measured and how to measure it again.
---

This page holds the measurements of the network: what one client of a game receives and sends per second, with and without each technique that saves bandwidth, and how to take the same measurements again. The figures come from real games played by the load-test bots against a real server: the rate and the differences are turned off by a launch option or a setting, the compression is read on both sides of the decoding of the client, and the packing of the fields, which never had a version without it, is computed from the measured figures and says so. The reactivity of the game under lag is not measured yet.

## What is measured

One client is one load-test bot (see [Load-test bots](/R-Type/network/#load-test-bots-load_test_bots)): it drives the `PlayerConnection` of the client, so it receives and sends what a client does, without prediction, and it confirms its inputs from the world states as the game screen does. Each bot adds up what crossed its socket during its time in game, from the first world state of a game to its end, and divides by that time:

| Figure | Counted by | Holds |
|---|---|---|
| Received, decoded | `NetworkThread::traffic()` | the bytes of each datagram once the client decoded what the server compressed |
| Received, on the network | `NetworkThread::wireTraffic()` | the bytes the network carried, before the decoding |
| Received with IP and UDP | computed | the bytes on the network plus 28 per datagram, the smallest headers of IPv4 and UDP |
| Sent | `NetworkThread::wireTraffic()` | what the client sent; it compresses nothing |
| Entities per state | `BotTally` | the entities a world state listed on average |

Every figure is in bytes of UDP payload per second unless the column says otherwise, and is the median over every bot of every run of a scenario. The counters and where they sit are in [Traffic counters](/R-Type/network/#traffic-counters-trafficcounters).

## The scenario

`tests/tools/bandwidth/campaign.sh` runs each scenario three times, one minute each: a game server on port 4310, run with `--mode server` so its games run in its own process and take its options (a supervisor would start each game in a process of its own, without them), started from an empty folder so no `server.cfg` applies, with its database in that folder and its journal off, and four bots, one full game, both from seed 1. The bots fly the same actions in the same order from one run to the next (see [Load-test bots](/R-Type/network/#load-test-bots-load_test_bots)); the server spawns the same waves. A run whose bots do not all end in session stops the campaign, and so does a bot that spent under 0.05 s in a game, which measured none (its time in game starts with its first world state and stops when the game ends), or a report line the script cannot read.

| Scenario | Server options | What it shows |
|---|---|---|
| `full-60` | `--state-rate 60 --set state-encoding=full` | 60 states per second, every one in full |
| `full-20` | `--set state-encoding=full` | the rate of 20, every state in full |
| `delta-20` | none | the server as it ships: 20 states per second, as differences |
| `full-20-bad` | `--set state-encoding=full --network bad` | full states over the `bad` simulated network |
| `delta-20-bad` | `--network bad` | differences over the same network |

`--set state-encoding=full` makes the server write every state with no reference (see [World state](/R-Type/server/#world-state-worldstatesender)). `--network bad` applies the `bad` scenario of the simulator to the server, in both directions: 300 ms more ping, 50 ms of jitter, 10 % of the datagrams lost, 2 % doubled, 5 % reordered (see [Network simulator](/R-Type/network/#network-simulator-simulatednetworksocket)). The bots run on a perfect network of their own.

To take the measurements again, build the test preset, then run the script from the root of the repository:

```bash
cmake --workflow --preset test
sh tests/tools/bandwidth/campaign.sh ./r-type_server ./build/tests/load_test_bots
```

The campaign takes about 15 minutes. It prints the folder that keeps the report of every run, then the table below. `sh tests/tools/bandwidth/campaign.sh ./r-type_server ./build/tests/load_test_bots 1 10` runs each scenario once, for 10 s, to try it.

## Results

Measured on 5 November 2026 on an Apple M4 Pro (12 cores) under macOS 27.0, at commit `f93ae54a` (protocol version 6), the server and the bots built by the `test` preset (Debug) with Apple clang 21.0.0 and CMake 4.3.2, on the loopback interface of that machine:

| Scenario | Entities per state | Received, decoded | Received, on the network | Datagrams received per second | Received with IP and UDP | Sent | Datagrams sent per second |
|---|---|---|---|---|---|---|---|
| `full-60` | 8.9 | 11191.8 | 7711.6 | 70.8 | 9696.8 | 2226.1 | 61.9 |
| `full-20` | 8.9 | 4009.9 | 2848.7 | 30.9 | 3713.8 | 2271.2 | 61.9 |
| `delta-20` | 8.9 | 3890.6 | 2830.1 | 30.8 | 3692.5 | 2269.4 | 61.8 |
| `full-20-bad` | 9.0 | 3984.4 | 2902.7 | 36.3 | 3919.2 | 2838.4 | 61.8 |
| `delta-20-bad` | 8.9 | 3992.8 | 2922.6 | 36.5 | 3942.4 | 2839.4 | 61.9 |

On the perfect network every bot received 20.0 states per second (60.0 in `full-60`) and sent 59.5 to 60.0 inputs per second, so the rhythm of a client held; over `bad` it received 17.0 to 17.6 states per second, the others lost on the way. No game ended during a run. From one bot to another, what a client received on the network varied by less than 1 % on the perfect network (2821.0 to 2834.4 B/s in `delta-20`) and by 3 to 4 % over `bad`. What a client sent varied more, from 2132.8 to 2536.4 B/s on the perfect network: a `PlayerInput` repeats every input the server has not applied yet (see [Inputs](/R-Type/protocol/#inputs)), and a bot confirms its inputs only from a state that lists its ship, so a bot whose ship is down repeats more; this cause is likely, not measured.

## What each technique saves

| Technique | Without | With | Saved |
|---|---|---|---|
| 20 states per second instead of 60 (`full-60` to `full-20`) | 7711.6 B/s on the network | 2848.7 | 63 % |
| Differences instead of full states, perfect network (`full-20` to `delta-20`) | 4009.9 B/s decoded, 2848.7 on the network | 3890.6, 2830.1 | 3.0 % decoded, 0.7 % on the network |
| Differences over `bad` (`full-20-bad` to `delta-20-bad`) | 2902.7 B/s on the network | 2922.6 | none: the gap, 0.7 %, is within the spread between bots |
| Compression of what the server sends (`delta-20`, decoded to on the network) | 3890.6 B/s | 2830.1 | 27 %, or 22 % with the headers of IP and UDP |
| Positions in 16 bits instead of two 32-bit floats, **computed** | about 4722 B/s decoded | 4009.9 (`full-20`) | 712 B/s, 15 % |
| Each action on one bit instead of one byte, **computed** | about 3837 B/s sent | 2269.4 (`delta-20`) | about 1570 B/s, 41 % |

- **The rate of states** is the largest lever: the bytes received grow with it, since the states are most of what a client receives.
- **The differences** save little in these games. An entity that moves costs 9 bytes in a difference as in a full state (see [Delta world state](/R-Type/protocol/#delta-world-state)), and the missiles and the Bydos move at every state, the ships whenever their pilot holds a direction: the differences saved 6.0 bytes per state before compression, less than one of the 8.9 entities. What they remove, the compression had mostly removed already, hence 0.7 % on the network. Over `bad` they save nothing measurable; a likely cause, not measured, is that a player acknowledges less, so the reference of a difference is older and the difference lists more.
- **The compression** takes out about a quarter of what the server sends (see [Compression](/R-Type/network/#compression-zstdcodec-compressingdatagramsocket)). Nothing the client sends is compressed.
- **What a client sends** is its inputs, 60 per second, each repeating the inputs the server has not applied yet. A `PlayerInput` takes 31 bytes and one more per repeated input; with about 85 B/s of other datagrams, the 2269.4 B/s of `delta-20` give about 5.5 repeated inputs each. Over `bad` the 300 ms of delay leave more inputs unconfirmed than a message holds, `MAXIMUM_REPEATED_INPUTS` (15), and every input carries 15: 2839 B/s.
- **The packing** has no version without it: positions are 16-bit eighths of a unit since the first world state was sent, and the actions are bits since the first `PlayerInput` (see [Positions](/R-Type/protocol/#positions)). Two 32-bit floats would add 4 bytes to each entity of each state: 4 × 8.9 × 20 = 712 B/s more before compression, from the measured `full-20`. One byte per action instead of one bit would add 4 bytes to each set of actions an input carries, its own and the repeated ones: 4 × 6.5 × 60, about 1570 B/s more sent. Both figures are computed, not measured; the first is an upper bound for differences, which list only the entities that moved, and the second rests on the estimate of the repeated inputs above.

## Limits

- The server and the bots run on one machine, over the loopback interface; a real network adds its own losses and delays, which the `bad` scenario only imitates.
- The bytes count the UDP payload; the column with IP and UDP adds the smallest headers of IPv4, 28 bytes per datagram. IPv6 adds 48.
- The bots predict nothing, as a client does without prediction; with prediction a client sends one input per simulation tick, 60 per second as well.
- A full state of `full-20` is a `DeltaWorldState` with no reference: its fixed part is 10 bytes, where the `WorldState` of the first versions of the protocol took 6. The figure without differences is therefore 4 bytes per state datagram above that of the old format, 80 B/s at 20 states per second.
- The server counts its own traffic above the compression (see [Traffic counters](/R-Type/network/#traffic-counters-trafficcounters)): its journal and its file give what a client would read decoded, not what the network carried.
- Four players and about nine entities per state: a game with more Bydos lists more entities, and every figure that depends on the states grows with them.
