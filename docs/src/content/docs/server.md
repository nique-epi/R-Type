---
title: Server and game
description: What the server program and the game library do, how their parts fit together and where to find them in the code.
---

The server, `r-type_server`, is meant to run the game for the players connected to it. Today it receives their datagrams, runs a tick at a fixed rate, hosts eight games at once, each letting up to four players in and starting when they are all ready: each tick answers the connection requests received and reads the `ReadyState` and the inputs of the players. Any datagram of a player proves it is still there, and a game marks a player it hears nothing from for 5 seconds as away, keeps its seat for 15 seconds more, then closes its session. The game tells its players the waiting room, the start of the game, the world state and, once no player has a life left, the end of the game with the score of each, each datagram under the session token and the next sequence of its player. Each tick of a game in progress moves the ship of every player by the arrows its newest input holds, fires the missiles of the players who hold shoot, lets every Bydo fire to the left, and runs the collisions, so three missiles destroy a Bydo, and a ship touched by a Bydo or by its missile costs a life. The Bydos come in waves that grow, read at launch from the waves file of the server. It never links SFML or Asio: it reaches the network through `rtype_network`, and its own libraries are checked for it at configure time (see [Project layout](/R-Type/project-layout/#who-may-link-what)).

The game library, `rtype_game`, holds the rules of a match: what moves, what touches what and what that means. It is built on the engine (see [Engine](/R-Type/engine/)), and it knows nothing about sockets, SFML or the messages sent between the programs (see [Project layout](/R-Type/project-layout/#game)). The server runs all of it; the client will only run the part that predicts its own ship.

This page tells you where to look when you need to change something. It describes what exists today and says plainly what does not exist yet.

| Part | State | Where |
|---|---|---|
| Network thread and simulation thread | Available | `src/server/Application/` |
| Timer resolution on Windows | Available | `src/server/Platform/` |
| The server assembled from modules (logger, network, games, lobby), stopped on Ctrl-C or SIGTERM with a `Disconnection` to every player | Available | `src/server/Modules/`, `src/server/Platform/StopSignals/` |
| Sorting the datagrams before the queues, session token checked | Available; no limit per address yet | `src/server/Application/DatagramSorter/` |
| World state sent at a fixed rate to the players of the game | Available | `src/server/Replication/WorldStateSender/`, `src/server/Instances/GameFlow/` |
| One header for every datagram to a player: session token and next sequence | Available | `src/server/Sessions/SessionDatagram/` |
| Game events sent to the players | Available: the removal of a Bydo or a missile that leaves the playfield, the damage and destruction of a Bydo hit by missiles, and the loss of a ship and of a life when a ship touches a Bydo or a missile of a Bydo | `src/server/Replication/`, `src/game/Events/` |
| Launch options | Available: the port, the seed and `--help` | `src/server/LaunchOptions/` |
| Letting players in: challenge, acceptance or refusal | Available; a new player joins the first game waiting for players with a free seat, and is refused with `GameInProgress` or `GameFull` only when no game has one | `src/server/Lobby/`, `src/server/Sessions/` |
| A game: states, ready rule, new world between two games | Available: eight games side by side, each ending once no player is left in it or no player left has a life | `src/server/Instances/GameFlow/` |
| Newest input of each player, applied to its ship at each tick of a game in progress | Available: the arrows move the ship, shoot fires its missiles | `src/server/Sessions/InputKeeper/`, `src/server/Instances/GameFlow/` |
| Heartbeats; a player silent for 5 seconds is away and keeps its seat 15 seconds, then its session closes | Available, and a `Disconnection` closes the session at once | `src/server/Sessions/` |
| A nickname unique on the server, held with the seat | Available: a second request for the same nickname gets `NicknameTaken` | `src/server/Sessions/PlayerRoster/`, `src/server/Lobby/` |
| Waiting room and start of the game sent to the players (`LobbyState`, `GameStarted`) | Available | `src/server/Replication/LobbyStateAnnouncer/`, `src/server/Instances/GameFlow/` |
| End of a game sent to the players (`GameEnded`) | Available: once no player has a life left, with the score of each, until the game waits for players again | `src/server/Replication/GameEndAnnouncer/`, `src/server/Instances/GameFlow/` |
| Several games, master link | Not yet written | — |
| Components (`Position`, `Velocity`, `CollisionBox`, `Health`, `EntityType`, `Sprite`, `Layer`) | Available | `src/game/Components/` |
| Movement, run by each game tick | Available | `src/game/Systems/MovementSystem/`, `src/server/Instances/GameFlow/` |
| Movement patterns, and removal of what leaves the playfield | Available: patterns for the Bydos; a Bydo and its missiles leave on the left, a missile of a player on the right | `src/game/Systems/PatternSystem/`, `src/game/Systems/PlayfieldExitSystem/` |
| Collisions | Available, run by each game tick | `src/game/Systems/CollisionSystem/`, `src/game/Systems/CollisionRules/`, `src/game/Events/`, `src/server/Instances/GameFlow/` |
| Damage and destruction | Available, run by each game tick; a missile takes no health from a ship, and what its hit costs belongs to the lives | `src/game/Systems/DamageRules/`, `src/server/Instances/GameFlow/` |
| Ship of each player, created when the player joins and announced to the others | Available | `src/game/Spawning/`, `src/server/Replication/ShipSpawnAnnouncer/` |
| Ship control: how a ship drifts from the directions its player holds, rolls and dashes on a double press, kept above the status band | Available; the client does not predict its own ship yet | `src/game/ShipControl/`, `src/server/Instances/GameFlow/`, `src/server/Instances/ShipInput/` |
| Bydos appearing off screen at random heights above the status band of the clients, in waves, then moving left | Available | `src/game/Spawning/BydoSpawnPosition.*`, `src/server/Spawning/` |
| Missiles of the players: fired in front of the ship while shoot is held, three per second at most, removed past the right edge | Available | `src/server/Spawning/MissileLauncher/`, `src/game/Spawning/MissileStartPosition.*`, `src/game/Systems/PlayfieldExitSystem/` |
| Missiles of the Bydos: every Bydo fires to the left 1.5 s after it appears, then every 1.5 s; removed past the left edge | Available | `src/server/Spawning/BydoGunner/`, `src/game/Spawning/EnemyMissileStartPosition/`, `src/game/Systems/PlayfieldExitSystem/` |
| Score: points per Bydo kind, credited to the player who fired the missile, sent in the world state | Available | `src/game/Spawning/BydoKind/`, `src/game/Systems/DamageRules/`, `src/server/Instances/ScoreKeeper/` |
| Waves of Bydos that grow, read from the waves file at launch | Available: every game plays the `normal` difficulty | `src/game/Waves/`, `src/server/Spawning/WaveCatalog/`, `assets/waves/waves.toml` |
| Lives: a life lost when a ship touches a Bydo or a missile of a Bydo, a new ship 1 s later that cannot be hit for 2 s, a player with no life left watching | Available | `src/server/Instances/LifeKeeper/`, `src/server/Instances/GameFlow/` |

## Launch options: `LaunchOptions`

`LaunchOptions` (`src/server/LaunchOptions/`) reads what the server is told at launch. `main()` first reads the logging options (see [Logging](/R-Type/logging/#choosing-the-level-and-the-output-at-launch)), then hands the other arguments to `LaunchOptions::read()`, which reads them with the engine's `readLaunchArguments` (see [Engine](/R-Type/engine/#launch-arguments-readlauncharguments)):

| Option | Effect |
|---|---|
| `-p <port>` | The UDP port to listen on, from 1 to 65535 (see [Network](/R-Type/network/#ports-and-addresses-written-as-text-portfromtext-ipv4addressfromtext)). Default: `DEFAULT_SERVER_PORT` (4242). |
| `--seed <seed>` | The number the random draws of the games start from, a whole number from 0 to 4294967295. Default: drawn at random. See [Bydos](#bydos-bydospawner). |
| `--help` | Prints the usage, the logging options included, on standard output, then exits with code 0 without starting anything or opening the journal. |

- The server listens on every IPv4 interface (`ANY_IPV4_ADDRESS`) and takes no address: `-h`, the client's address option, is an unknown argument here. The last `-p` wins, and so does the last `--seed`.
- An unknown argument, a `-p` or `--seed` without a value, an invalid port or a seed that is not a whole number from 0 to 4294967295 stops the server before anything starts: it prints `r-type_server:`, the reason and how to see the usage on standard error, then exits with code 1.

```bash
./r-type_server -p 4243
```

## Threads: `ServerApplication`

`ServerApplication` (`src/server/Application/ServerApplication/`) runs the server on two threads, which never share game state:

- The **network thread**, the one that calls `run()`, runs the `NetworkContext`. A `DatagramSorter` sorts every datagram as it arrives: the datagrams of admitted players that carry their session token go to the player queue, connection requests to the request queue, and the rest is dropped there (see [Sorting the datagrams](#sorting-the-datagrams-datagramsorter)). `DatagramRelay` (`rtype_network`, see [Network](/R-Type/network/#socket-to-queues-datagramrelay)) sends what was left in the outgoing queue. It is the only thread that touches the socket.
- The **simulation thread** runs `SIMULATION_TICKS_PER_SECOND` (60) ticks per second. Each tick drains the player queue and the request queue and hands what arrived, often nothing, to the tick handler given to the constructor, as a `ReceivedDatagrams` (`players`, `connectionRequests`), so a slow or silent player never holds the game up. At the end of the tick it asks the network thread, through `NetworkContext::post()`, to send the outgoing queue.

The tick handler is where the game plugs in. The network module gives one (see [Modules](#modules-and-stopping-the-server)): it keeps the datagrams for the modules and runs the stages of the loop, so in a tick each game of the list gets the datagrams of its own players, reads the `ReadyState` and the inputs among them, moves along its states and tells its players the room, the start and the world state (see [A game](#a-game-gameinstance-and-gameflow) and [Player inputs](#player-inputs)), and the address of each player whose session closed in that tick is forgotten (see [Silent players](#silent-players-heartbeats-and-timeouts)); then each connection request goes to the `ConnectionAcceptor` of the game it is placed in, which answers it and gives each new player a ship in that game (see [Letting players in](#letting-players-in-connectionacceptor-playerroster) and [Player ships](#player-ships-shipspawnannouncer)); then the timers advance by one tick and the `EventBus` of each game delivers its events, which the broadcaster sends to the players who joined (see [Game events](#game-events-gameeventencoder-and-gameeventbroadcaster)).

A tick handler puts a datagram in the outgoing queue with `queueDatagram()`, called from the handler itself, on the simulation thread. The datagram is sent at the end of the same tick.

The sorter only lets through datagrams of this protocol whose message sizes add up, but anyone can still send a connection request, and the payloads are read by the tick, so the tick handler must drop and count a datagram it cannot decode, and never let an exception caused by a datagram's content escape: an exception stops the server (see below), so one truncated datagram would stop it for every player. `ByteReader`, for instance, throws `BufferUnderflowException` when a datagram is shorter than what it reads.

The player queue and the outgoing queue hold `INCOMING_DATAGRAM_QUEUE_CAPACITY` and `OUTGOING_DATAGRAM_QUEUE_CAPACITY` (256) datagrams each, about one second of the inputs of four players; the request queue holds `CONNECTION_REQUEST_QUEUE_CAPACITY` (16) (`src/server/ServerConstants.hpp`). These sizes are **provisional**: they will be revised with the rates of the protocol. Code must read the constants.

- When more datagrams arrive between two ticks than the player queue or the request queue holds, the oldest are discarded. Every `TICKS_PER_REPORT` ticks (600, so every ten seconds), the server logs one warning per queue with how many were discarded, if any, one with how many datagrams the sorter dropped before any queue, and one with how many it dropped for their session token, naming the address and port of the first of them (see below). Each datagram the sorter drops is also logged with its sender at the `debug` level (see below).
- A flood of connection requests, or of datagrams from strangers, never pushes a player's datagram out of its queue. It still delays the requests of new players: nothing limits the requests of one address yet (see [Flood protection](#flood-protection)).
- The outgoing queue also discards its oldest datagram when full, but nothing reports it yet.

`ServerApplication` implements `IDatagramSender` (`queueDatagram()`) and `IPlayerAdmission` (`admit()`, `forget()`), the two interfaces the other parts of the server use to reach it. Building it binds nothing: `open()` binds the socket (`ServerAlreadyOpenException` when it is already bound), and `run()` and `localEndpoint()` throw `ServerNotOpenException` before it. `close()` unbinds it, so the port is free again once `run()` returned.

`stop()` makes `run()` return, from any thread: the network thread stops at once, the simulation thread when its current sleep ends, about one tick later. An exception thrown on either thread, by the tick handler for instance, stops both, and `run()` rethrows it.

### Sorting the datagrams: `DatagramSorter`

`DatagramSorter` (`src/server/Application/DatagramSorter/`) decides, on the network thread, which queue each received datagram goes to, before any queue holds it. It reads the header and the type of each message, never a payload. The first row that applies decides where a datagram goes:

| The datagram… | Goes to |
|---|---|
| is of this protocol with another version, from a sender that was not admitted, and holds at least `CONNECTION_REFUSED_DATAGRAM_SIZE` (26) bytes | the request queue, where the `ConnectionAcceptor` refuses it |
| is not of this protocol and version, has a flag set, or its message sizes do not add up | nowhere: dropped |
| comes from the address and port of an admitted player without the session token of that player (a `ConnectionRequest` may carry 0 instead) | nowhere: dropped, and counted apart |
| holds a `ConnectionRequest`, whoever sent it | the request queue, where the `ConnectionAcceptor` answers it (see [Letting players in](#letting-players-in-connectionacceptor-playerroster)) |
| comes from an admitted player | the player queue |
| anything else | nowhere: dropped |

- A player is admitted by `ServerApplication::admit(endpoint, sessionToken)`, which the acceptor calls once for each sender it gives a place to, with the token it drew for that player; admitting an address again replaces its token. It may be called from any thread: it posts the admission to the network thread, so a datagram the player sends before that is dropped like a stranger's. `ServerApplication::forget(endpoint, sessionToken)` undoes it once the session of that player closed (see [Silent players](#silent-players-heartbeats-and-timeouts)): the datagrams from that address are dropped like a stranger's again, unless the address was admitted with another token in between, so the two calls may reach the network thread in either order.
- The `PlayerRoster` and the `ConnectionAcceptor` run in the tick, on the simulation thread; the network thread only keeps the address, port and session token of each admitted player.
- Each datagram the sorter drops is counted, readable from any thread: `wrongTokenCount()` for those dropped for their session token, `droppedCount()` for the others. The server reports both every ten seconds with the discards of its queues. The line about the tokens names the address and port of the first such datagram since the previous line (`takeWrongTokenSender()`): the address of a player, used by someone who does not know its token. A flood of forged datagrams therefore adds one line every ten seconds, never one per datagram. Besides these counts, every drop, a wrong token included, is logged at the `debug` level with its sender and why, such as `it is shorter than the packet header` or `its sender was not admitted, and it holds no connection request`, at most once per `REJECTION_LINE_INTERVAL` (10 s), with how many since the previous line (see [Logging](/R-Type/logging/#repeated-events-throttledlog)). The time of these lines comes from the `IClock` the server application gives the sorter, read only to drop a datagram.
- Measured on an Apple Silicon laptop (Apple M4 Pro, `-O2`, median of 7 runs of 2 million datagrams): sorting a datagram costs about 30 ns when a stranger's is dropped, 29 ns when one is dropped for its session token, the sender kept included, 18 ns for bytes shorter than a header, and 37 ns when it is queued, copy included. Reading the clock and counting the drop for its log line take 8 to 16 ns of a drop. A flood of 3 800 datagrams per tick therefore costs the network thread under 0.15 ms of every 16.7 ms, whatever the datagrams hold. A malformed datagram is found without throwing (`DatagramCodec::messageTypes`): an exception costs about 4 µs, so a flood of malformed datagrams would otherwise take about 14 ms of every tick. Answering a request costs about 0.45 µs, or 0.55 µs with a wrong cookie; the request queue keeps that under 9 µs per tick.

### Flood protection

The room of the player queue protects the server's memory; the sorter protects the players from the senders that were not admitted: one of them flooding the server's port with connection requests or with anything else no longer pushes the players' datagrams out. Someone who forges the address and port of an admitted player no longer can either, unless they read its session token on the way: the sorter drops their datagrams before the queue. That player itself, flooding with its own token, still can: the players share one queue (layer 2 below). Since a game closes the session of a player it hears nothing from for 20 seconds, a player who floods long enough can even make the others time out: in the waiting room, where a client sends one heartbeat per second, every heartbeat a player sends for 20 seconds, about twenty, would have to be pushed out of the queue (an estimate, not measured). A plain Python loop on the loopback address sends about 3 800 datagrams per tick (measured on an Apple Silicon laptop), about fifteen times the room of the queue. Other engines protect the players in three layers; the server must have them once sessions exist, and the first and the third are written, in part:

1. **Sort on the network thread, before the queue.** Written: `DatagramSorter` drops there every datagram that is neither from an admitted player nor a `ConnectionRequest`, and every datagram from the address of a player without its session token; the requests have their own queue. Not written: a limit on the requests of each address, in a table of fixed size so that forged addresses cannot make it grow. Quake 3 ignores such datagrams and answers at most 10 status, information or remote console requests per second from one address; Unreal limits how many datagrams of senders without a connection it processes each frame.
2. **One queue per session**, so a flooding client only fills its own. Valve's GameNetworkingSockets keeps one per connection, of 1 000 messages by default.
3. **One input per player and per tick.** The protocol requires it (see [Protocol](/R-Type/protocol/#inputs)). Written: each `GameInstance` keeps only the newest `PlayerInput` of each of its players and ignores older ones (see [Player inputs](#player-inputs)), then applies it to the ship of that player at each tick of a game in progress (see [Moving the ships](#moving-the-ships-shipcontrol)).

The player queue stays as the last safety net. It discards its oldest datagram, where GameNetworkingSockets refuses the newest, because only the newest input of a player counts. Why threads only talk through queues is explained in [Architecture](/R-Type/architecture/#queues-between-threads).

## Modules and stopping the server

`src/server/main.cpp` reads the options and hands a list of modules to an `engine::Application` (see [Engine](/R-Type/engine/#modules-application-and-imodule)). `serverModules()` (`src/server/Modules/Assembly/ServerModules.cpp`) is the one place that lists them; they start in this order and stop in the reverse one.

| Module | Does | Where |
|---|---|---|
| `logger` | applies the logging options when it starts, logs "server starting" and "server stopped" | `src/server/Modules/Logger/` |
| `network` | owns the `ServerApplication`: binds the socket and runs the server on a thread when it starts, gives the port back when it stops; provides `IDatagramSender`, `IPlayerAdmission` and `TickDatagrams` (the datagrams of the tick) | `src/server/Modules/Network/` |
| `games` | the games (`GamesSettings::gameCount`, `DEFAULT_HOSTED_GAME_COUNT` = 8), an `EventBus` for each and the `TimerScheduler` they share; reads the waves file when it starts; provides `Games` | `src/server/Modules/Games/` |
| `lobby` | one `ConnectionAcceptor` per game, built on the roster of that game | `src/server/Modules/Lobby/` |

Each tick runs the stages of `SERVER_LOOP_STAGES` once, with `SIMULATION_TICK_DURATION`. In `simulation`, the games module runs `onTick()` on each game with the datagrams of its own players (`keepDatagramsOf()`) and makes the network forget the players whose session closed, then the lobby module answers the connection requests, so a request is placed and refused by the state the games just reached. In `outputs`, the games module advances the timers and the bus of each game delivers its events. The `inputs` stage is empty: `onTick()` reads the datagrams and moves the game in one step.

### Several games at once

The server creates its games when it starts and keeps them: a finished game waits for players again with a new world, so no game is ever replaced. `Games` decides where a connection request goes, the same way for the lobby and for the tests:

| The sender | The request goes to |
|---|---|
| has a seat in a game | that game, so a repeated request gets the same acceptance again |
| has no seat | the first game waiting for players with a free seat (`Games::joinable()`) |
| has no seat, and no game waits with a free seat | the first game, which refuses it with `GameInProgress` or `GameFull` |

The rule itself is `firstJoinableGame()` (`src/server/Lobby/GamePlacement/`), which reads a `GameVacancy` per game (its state, and whether it is full) rather than the games, so a process that does not own the games could place players with it. Each game has its own acceptor, which draws its own cookies: a player challenged by one game and placed in another, because the first one filled up meanwhile, is challenged once more.

Each game has its own `EventBus`, since its rules and its broadcaster subscribe to it: with one bus, a game would apply the collisions of another to its own entities and send its events to the wrong players. The timers are shared, since each one calls back into the game that set it. The seed of game n is the seed given plus n, so a game can be played again; without a seed, each game draws its own.

Every game runs on the simulation thread, one after the other in each tick, and the network thread stays the only other one. No state is shared between games, so nothing needs a lock, and the queues between the two threads do not change. The cost of a tick grows with the number of games and is not measured yet: the tick loop reports its late ticks every 10 s. An exception in one game stops the server, and a slow game slows the others; running each game in a process of its own would isolate them.

The waves file is read when the games module starts, after the logger module started, so a missing or wrong file stops the start with an error that names the games module, the file, the line and the key, and the line "waves file …" reaches the journal.

### Stopping: Ctrl-C and SIGTERM

`StopSignals` (`src/server/Platform/StopSignals/`) turns the signals that ask a program to stop into a request on a `StopRequest`, the object `main()` waits on. A failure of the network thread asks for the same stop, with the text of the error, so `main()` returns 1 after it and 0 after a signal. Only the first request counts.

- Linux and macOS: `SIGINT` (Ctrl-C) and `SIGTERM`. `StopSignals` blocks them in the main thread before any other thread exists, so every thread inherits the mask, and one thread takes them with `sigwait()`: nothing runs inside a signal handler. A signal that arrives before it is built, while the program starts, has its default action.
- Windows: `SetConsoleCtrlHandler`. The system runs the handler on a new thread. Ctrl-C and Ctrl-Break have no time limit; closing the console window gives the process 5 seconds, so that handler waits for the program to finish stopping, at most `CONSOLE_CLOSE_WAIT` (4 s), before it returns. Logoff and shutdown only reach services, and are handled the same way. This file is checked by the Windows job of the CI only.

On a request, `main()` calls `Application::stop()`. It first stops the stages: it waits for the tick in progress, and no work runs after it. Then the modules stop in the reverse order. The games module tells every player that the server shuts down, with `REPEATED_MESSAGE_COPIES` (3) copies of a `Disconnection`, `REPEATED_MESSAGE_INTERVAL` (50 ms) apart, each under the next session header of its player (see `FarewellSender`), then waits `FAREWELL_FLUSH_WAIT` (two ticks) so the last copy leaves; the whole stop takes about 130 ms with a player connected (two intervals and two ticks). The network module then stops the server and closes the socket, and the journal logs "server stopped". The client does not read that `Disconnection` yet (see [Protocol](/R-Type/protocol/#a-peer-disappears)).

## Tick loop: `TickLoop`

`TickLoop` (`src/server/Application/TickLoop/`) calls a function once per tick on the thread that runs it. It counts ticks with a `FixedTimestep` and sleeps until its `nextTickTime()`, so the average rate stays at 60 ticks per second: after a late wake-up the missed ticks run back to back, and a stall longer than `MAXIMUM_TICKS_PER_ADVANCE` (5) ticks drops the excess. `ServerApplication` creates it when `run()` starts, so the first tick is due one tick after the call.

The limit is a choice for this game: during a longer stall the game freezes, then resumes where it stopped, unless it lasts long enough for every client to take the server for lost (see [A long stall and the clients](#a-long-stall-and-the-clients)). Catching the whole stall up would play it in an instant, a second of enemy fire that nobody saw coming. Engines differ on it: a Quake 3 dedicated server catches up to 5 seconds, so that the clients' view of the server's clock stays right; Unity lets each game set its `maximumDeltaTime`.

Every `TICKS_PER_REPORT` ticks, the loop logs at the `debug` level how late its ticks started, each compared with the moment it was due; a dropped stall shows in full. The lines go to the terminal and to the journal, `r-type_server.log` (see [Logging](/R-Type/logging/#choosing-the-level-and-the-output-at-launch)):

```bash
./r-type_server --log-level debug
```

After the time stamp, the line reads:

```
[DEBUG] [Simulation] - 600 ticks started on average 4.43782 ms late, at most 8.32842 ms
```

Every stall that drops game time is counted, and the same report then adds a warning, shown at the default level: how many stalls dropped game time since the previous report, how much in total, and the longest. Measured on a server paused for one second:

```
[WARN ] [Simulation] - stalls since the previous report: 1, game time dropped: 939.018 ms, the longest: 939.018 ms; the game froze instead of catching up
```

Repeated at every report, it means that the server keeps falling behind, for instance because its ticks take longer to run than the 16.7 ms each one stands for.

How late a tick starts depends on how precisely the system wakes a sleeping thread:

- On Windows, MSVC's `sleep_for` ends in `Sleep`, whose documentation says that a wait longer than one tick of the system clock but shorter than two "can be anywhere between one and two ticks". While it runs, the loop holds a `FineTimerResolution` (`src/server/Platform/`), which asks Windows for `FINE_TIMER_RESOLUTION_MILLISECONDS` (1 ms). If Windows refuses, the loop logs a warning and runs anyway.
- On macOS, measured on an Apple Silicon laptop, a sleep wakes later the longer it is: 0.5 ms late on average for a 1 ms sleep, 5.6 ms for a 16.7 ms sleep. Timer coalescing, enabled on that machine, is the likely cause. Over several 30-second runs, ticks started 3.8 to 4.5 ms late on average and at most 9.1 ms late, with no client as with four clients sending 60 datagrams per second each, and 600 ticks still ran every ten seconds.
- On Linux and Windows, the lateness is not measured yet.

## World state: `WorldStateSender`

`WorldStateSender` (`src/server/Replication/WorldStateSender/`) sends the state of the game to its players, at a fixed rate. The `GameReplication` of each game builds one with its `PlayerRoster`, and `GameFlow` has it call `onTick(tick, components)` on the tick that starts the game and on every tick of the game, on the simulation thread, with the tick the game counts (see [A game](#a-game-gameinstance-and-gameflow)). Every `TICKS_PER_WORLD_STATE` (3) ticks, that is `WORLD_STATES_PER_SECOND` (20) times per second, it lists every entity that has a `NetworkIdentity`, a `Position` and an `EntityType`, has `WorldStateCodec` encode the list (see [Network](/R-Type/network/#world-state-worldstatecodec)), and hands one `OutgoingDatagram` per connected player and per payload to the function it was given, each starting with the header of that player (see [Session headers](#session-headers-sessiondatagram)). In `main.cpp` that function is `queueDatagram()`.

- The `tick` of a state is the tick given to `onTick`, so the client can tell which of two states is newer. The first state of a game carries tick 0 and leaves in the tick that starts the game, right after `GameStarted`. The game's counter is 32 bits and is not made to wrap: it takes 2.27 years at 60 ticks per second. It starts again at 0 with each game.
- `WORLD_STATES_PER_SECOND` is **provisional**: it will be revised once measured on a real network. It must divide the tick rate, and the build fails otherwise. Code must read the constants, `TICKS_PER_WORLD_STATE` for instance, and never repeat the 3.
- Nothing is built when no player is connected. An entity without a network id, or without a type, is not listed.
- An entity whose position is not a finite number (NaN or infinite), which only a game bug can produce, is left out of the state instead of stopping the server. A warning under the `Replication` module gives how many entities were left out, each time that number changes, so a stuck entity is reported once rather than 20 times per second.
- The state lists every connected player with its number, ship, and score. `lives` is what the player has left, `invulnerable` is true while its ship, back after a hit, cannot be hit (see [Lives](#lives-lifekeeper)), and `motion` holds the directions of its newest input and whether its ship rolls or dashes, so every client can draw what each pilot does. The score is the points the player earned in the current game: a missile with a `MissileOwner` that destroys an enemy with a `ScoreValue` makes `DamageRules` publish `EnemyKilled`, and the `ScoreKeeper` adds the points (`DRIFTER_POINTS` 100, `WEAVER_POINTS` 200, both provisional) to the player's score, which stops at its largest value and goes back to 0 with each new world. The ship of each player has a `NetworkIdentity` (see [Player ships](#player-ships-shipspawnannouncer)), so it is listed as an entity too.
- A state split into several datagrams takes one sequence per datagram, so the client reads every part.
- A playfield larger than 2047 logical units in either direction fails the build, because a wire coordinate would no longer hold it (see [Protocol](/R-Type/protocol/#positions)).

```cpp
rtype::server::WorldStateSender worldStateSender{roster, send};
worldStateSender.onTick(tick, world.components());
```

`GameReplication` builds the sender once, with its roster, which lives as long as the game, and `GameFlow` hands it the components of the current world at each call: a new world replaces the old one between two games.

`World::components()` (`rtype_game`) is the `ComponentRegistry` of the world: a `World` owns its entities and their components, and cannot be copied or moved.

## Letting players in: `ConnectionAcceptor`, `PlayerRoster`

`ConnectionAcceptor` (`src/server/Lobby/ConnectionAcceptor/`) answers the connection requests the sorter set aside for each tick, at most `CONNECTION_REQUEST_QUEUE_CAPACITY` (16), as the [Protocol](/R-Type/protocol/#connection) describes, and `PlayerRoster` (`src/server/Sessions/PlayerRoster/`) holds the players it let in: at most `MAXIMUM_PLAYERS` (4), each a `ConnectedPlayer` with its number, nickname, address and port, session token, next sequence, ship id, score, newest input, and the ticks since the game last read one of its datagrams and since the server last sent it one. A new player takes the lowest free number, so the first four are 1 to 4 in the order they joined. `remove(endpoint)` frees the seat and the number of a player whose session closed; the next player to join takes the lowest free number again.

A datagram of this protocol with another version is answered with `ConnectionRefused` (`IncompatibleVersion`) in a header of version 1, whatever it holds, when it holds at least `CONNECTION_REFUSED_DATAGRAM_SIZE` (26) bytes, the size of that refusal, so the answer is never larger than the datagram; a shorter one gets nothing. Any other datagram is read only when its header is of this protocol and version and its message sizes add up; only its first `ConnectionRequest` is answered, so one datagram never gets more than one answer, and every other message is ignored. A request is answered by the first row that applies:

| The request comes… | Answer |
|---|---|
| from a connected player | the same `ConnectionAccepted` again (same number, same token) with a valid cookie; a `ConnectionChallenge` without one, even during a game |
| while a game is in progress | `ConnectionRefused` (`GameInProgress`) |
| when four players are connected | `ConnectionRefused` (`GameFull`) |
| when a player holds a seat under the same nickname, away or not | `ConnectionRefused` (`NicknameTaken`); letters of the ASCII alphabet compare without regard to case (`PlayerRoster::hasNickname`) |
| without a valid cookie | `ConnectionChallenge`, and nothing is kept |
| with a valid cookie | a place in the roster, a session token drawn at random (never 0), and `ConnectionAccepted` |

- A refusal reserves nothing, so it goes before the cookie is checked. The cookies come from a `CookieIssuer` whose key is drawn when the acceptor is built (see [Network](/R-Type/network/#connection-cookies-cookieissuer)).
- An acceptance carries the session token and the next sequence of that player, 0 for the first (see [Session headers](#session-headers-sessiondatagram)). Challenges and refusals carry token 0 and sequence 0, as every datagram sent before a session.
- Besides the function that admits a sender, the acceptor takes an optional one it calls once for each player it lets in, after it sent the acceptance, and not again when an acceptance is repeated. The lobby module gives the acceptor of each game that `GameInstance`, which gives the player a ship.
- Nothing caused by the content of a datagram makes it throw. Each new player is logged at the `info` level under `Lobby`, as `player <n> joined from <address> port <port>`; challenges and refusals at the `debug` level. A datagram it cannot read, or whose request its reader refuses (a nickname empty, longer than 16 bytes, not UTF-8 or holding a control character, a cookie cut short), gets no answer and is logged at the `debug` level with its sender and why, at most once per `REJECTION_LINE_INTERVAL` (10 s) of the time the tick hands it.
- Each new player is announced once to the function given to the acceptor, which the lobby module hands to `IPlayerAdmission::admit()` (implemented by `ServerApplication`), so the datagrams of that player reach the tick from then on.
- `setGameInProgress(bool)` turns the `GameInProgress` refusal on and off. The lobby module sets it on the acceptor a request is placed with, just before that acceptor reads it, to whether its game is in any state other than waiting for players, so a request that arrives in the tick where the game starts goes to another game. A player leaves the roster when its session closes (see [Silent players](#silent-players-heartbeats-and-timeouts)).

## Session headers: `SessionDatagram`

Every datagram the server sends a connected player starts with the session token and the next sequence of that player, whatever its message, because the client drops a datagram that carries another token or that is not newer than the newest one it read (see [Protocol](/R-Type/protocol/#datagram)). The free functions of `SessionDatagram.hpp` (`src/server/Sessions/SessionDatagram/`) are the one place that reads and moves `ConnectedPlayer::nextSequence`. They take the server's `ConnectedPlayer`, so they live in the server, in the sessions folder, rather than with the session checks of `rtype_network`:

- `nextSessionHeader(player)` gives the header and moves the sequence on by one, wrapping from 65535 to 0. It also starts over the count of ticks since a datagram was sent to that player (`ConnectedPlayer::ticksSinceSent`), from which the game sends its heartbeats (see [Silent players](#silent-players-heartbeats-and-timeouts)).
- `sessionDatagram(player, message)` builds the datagram to the address of the player; an overload takes a message type and a payload already encoded, as the world state needs.
- `sessionHeaderFor(roster, endpoint)` gives the header of the player who sends from that address, or a header without session (token 0, sequence 0) when no player does.

The acceptance, the `EntitySpawned` of ships and Bydos, the waiting room, `GameStarted`, the world state and the game events all go through them, so the sequences a player receives grow by one from its acceptance, across every kind of message.

```cpp
send(rtype::server::sessionDatagram(player, rtype::network::GameStarted{}));
```

## Sessions of a game: `GameSessions`

`GameSessions` (`src/server/Sessions/GameSessions/`) holds the players of one game and the sessions the server keeps with them. `Sessions/` links no other folder of `src/server`, so `Instances/`, `Lobby/`, `Replication/` and `Spawning/` link it and never the other way round. It is not yet free of R-Type: `ConnectedPlayer` still carries the ship, score and lives of a player, and `Departures` publishes the `PlayerDeparture` of `src/game`. A game gives it a `GameSessionsParts`: the event bus, the send function, `readyAccepted`, which says whether a `ReadyState` counts right now, `onClosing`, which receives each player whose session closes, and `onNewerInput`, which is shown each `PlayerInput` newer than the one kept, before it replaces it.

| Class or function | Does |
|---|---|
| `DatagramReader` | splits a datagram of a player into its messages, or drops it and logs why |
| `ReadyKeeper` | keeps whether each player is ready, and whether all of them are |
| `InputKeeper` | keeps the newest `PlayerInput` of each player, after showing it to `onNewerInput` |
| `Departures` | closes the session of a player who left or fell silent, and keeps it for `takeDepartedPlayers()` |
| `sendHeartbeats` | sends a `Heartbeat` to a player the server sent nothing for `TICKS_PER_HEARTBEAT` ticks |

- `GameInstance` calls `readDatagrams`, `closeSilentSessions`, `endTick` (the heartbeats, then the tick is counted) and `takeDepartedPlayers`; `GameFlow` calls `everyPlayerIsReady` through the function `GameInstance` gives it; the rest of this page tells what each does.
- When a session closes, `Departures` frees the seat, calls `onClosing` with a copy of the player (the game removes its ship there), then logs and publishes the `PlayerDeparture`, in that order.
- The log lines of the sessions are written under `Sessions`.


## A game: `GameInstance` and `GameFlow`

`GameInstance` (`src/server/Instances/GameInstance/`) is the context of one game: its `PlayerRoster` (players 1 to 4, whose numbers also pick their colors), the sessions of its players (see [Sessions of a game](#sessions-of-a-game-gamesessions)), its `SystemScheduler`, the event bus and the timers it is given, and its modules (see [Modules of a game](#modules-of-a-game-igamemodule)). It knows nothing of the rules it plays: the `RtypeRulesModule` carries them out and owns the world of the game, in a `WorldSlot` (the current `World`, which a new one replaces between two games), with a `GameFlow` (`src/server/Instances/GameFlow/`) that is how the game runs: its state, its tick and every part that works on one world. The game has no network of its own, so a rule changes there without touching how players connect or what they send. The server keeps a list of games (`Games`, see [Several games at once](#several-games-at-once)). The tick handler calls `onTick(datagrams)` on each game with the datagrams of its own players, on the simulation thread, and `onPlayerJoined(player)` gives a new player its ship.

| State (`GameState`) | The game… | It goes to… |
|---|---|---|
| `WaitingForPlayers` | takes `ReadyState`, lets players in and sends them the room (see [Waiting room](#waiting-room-lobbystateannouncer)) | `InProgress` on the tick when at least one player is connected and every connected player is ready, which sends every connected player `GameStarted` |
| `InProgress` | counts one tick per `onTick`, moves the ships, runs the systems of the world, fires the missiles of the players who hold shoot, lets the Bydos fire, lets the Bydos appear, counts the lives and sends the world state; refuses new connections | `Finished` when `finish()` is called, on the tick the session of its last player closes, or on the tick after the last life of its players is lost (see [End of a game](#end-of-a-game-gameendannouncer)) |
| `Finished` | refuses new connections; a game that ended by itself sends every player the result (see [End of a game](#end-of-a-game-gameendannouncer)) | `WaitingForPlayers` after `FINISHED_STATE_TICKS` (300) ticks, `FINISHED_STATE_SECONDS` (5) seconds |

- Going back to `WaitingForPlayers` builds a new `World`: the tick is 0, the players stay connected, none is ready any more, and each gets its `STARTING_LIVES` back and a new ship, announced again. Network ids start again at 1 in the new world, so the announced ids are the ones of the previous game.
- `onTick` reads a datagram only when its header is of this protocol and version, its message sizes add up and its sender is a player of the game. It does not check the session token: the sorter already dropped, on the network thread, every datagram from the address of a player that does not carry its token (see [Sorting the datagrams](#sorting-the-datagrams-datagramsorter)), so a sender that forges the address of a player can neither say ready nor steer in its place. Every datagram it does not read is logged at the `debug` level under `Sessions` with its sender and why, at most once per `REJECTION_LINE_INTERVAL` of the ticks the game ran: 600 ticks count as exactly 10 s.
- A `ReadyState` counts only while the game waits for players, and with a sequence newer than the newest one accepted from that player (`isNewerSequence`, 1 to 32767 ahead, wrapping from 65535 to 0; exactly 32768 ahead is never newer). A late copy therefore never undoes a newer one. Anything else is ignored.
- `finish()` ends the game in progress and does nothing in another state; it tells the players no result. The game ends itself the same way once no player is left in it (see [Silent players](#silent-players-heartbeats-and-timeouts)) or once no player left in it has a life (see [End of a game](#end-of-a-game-gameendannouncer)), and only then tells them the result.
- The tick that starts the game sends every connected player `GameStarted`, then the world state of tick 0, so `GameStarted` always leaves first and a lost one is corrected by that world state. The game logs `game started, players connected: <n>` at the `info` level under `Instances`.
- The end of a game goes out as a `GameEnded` (see [End of a game](#end-of-a-game-gameendannouncer)).

```cpp
for (const std::unique_ptr<GameInstance>& game : games_.all()) {
  game->onTick(tickDatagrams_->players());
  for (const ConnectedPlayer& departed : game->takeDepartedPlayers()) {
    admission_->forget(departed.endpoint, departed.sessionToken);
  }
}
```

The simulation stage runs these steps (see [Modules](#modules-and-stopping-the-server)); `ServerLobbyModule` hands each request to the acceptor of the game it is placed in, after telling that acceptor whether its game is in progress.

## Modules of a game: `IGameModule`

A game installs modules instead of building the rules of R-Type itself, so a rule is added without touching `GameInstance`, and a game of another kind lists its own modules. `IGameModule` (`src/server/Instances/IGameModule.hpp`) has four functions: `name()`, `install(game, install)`, `start()` and `stop()`. `GameModules` (`src/server/Instances/GameModules/`) installs them in the order they are listed, starts them in that order, and stops the started ones in the opposite order when the game is destroyed. A module whose `start()` throws is not stopped, and neither are those after it. A null module in the list throws `NullModuleException` before any module is installed, through the check `Application` makes on its own modules (`requireNoNullModule`, `src/engine/Modules/RequireNoNullModule.hpp`).

- `install()` takes what the module needs from the `GameContext` (the bus, the timers, the systems, the roster, the session hooks, the settings (a copy of the seed and the waves), the send function and the ready rule), provides its services and adds its work to the stages of the tick. It keeps nothing open: subscriptions and the parts that work on the world are made in `start()` and released in `stop()`.
- A module that needs the service of another must be listed after it: `install.require<IGameReplication>()` throws `MissingServiceException` otherwise.
- A tick runs three stages, in order: `GAME_INPUT_STAGE` reads the datagrams and closes the silent sessions, `GAME_SIMULATION_STAGE` runs what the modules added, `GAME_OUTPUT_STAGE` sends the heartbeats.
- One module must provide `IGameFlow` (`state()`, `tick()`, `finish()`); the game answers its own functions through it.
- `RtypeRulesModule` builds its `GameFlow` in `install()`, which opens nothing, and `start()` calls `GameFlow::begin()`, which builds the first world and subscribes the rules to the bus. Until then the flow answers as a game that waits for players at tick 0. `stop()` destroys the flow, then the `ScoreKeeper`, which give their subscriptions back to the bus.
- What happens to a player goes through `SessionHooks` (`src/server/Instances/SessionHooks.hpp`), in the `GameContext`: three lists of functions, `joined`, `closing` and `newerInput`. A module adds its functions in `start()`, and the game calls them in the order of the modules, so any module can follow a player joining, a session closing or an input being kept, without a change to `GameInstance`. The lists are not emptied by `stop()`: the game is destroyed right after its modules stop and calls no hook in between.

| Module (`rtypeGameModules()`, in this order) | Provides | Holds |
|---|---|---|
| `ReplicationModule` (`src/server/Instances/ReplicationModule/`) | `IGameReplication` | `GameReplication` (the world state sender and the announcers of the room, the ships and the end of a game) and the `GameEventBroadcaster`, subscribed when the module starts |
| `RtypeRulesModule` (`src/server/Instances/RtypeRulesModule/`) | `IGameFlow` | the world of the game (its `WorldSlot`), the systems of the world, the `GameFlow` with its rules, and the `ScoreKeeper` |

`GameFlow` tells the players what happens through `IGameReplication`, never through the announcers: each call sends what it has to before it returns, so the order of the datagrams to a player is the order of the calls. The bus cannot carry these calls, because it delivers an event at the end of the tick, while creating a ship must set `ConnectedPlayer::shipEntityId` at once.

```cpp
rtype::server::GameInstance game{
    rtype::server::GameInstanceParts{.send = send,
                                     .settings = settings,
                                     .events = events,
                                     .timers = timers},
    rtype::server::rtypeGameModules()};
```

## Silent players: heartbeats and timeouts

A client that crashes, is killed or loses the network sends nothing more, and nothing in UDP says so. Each game's `GameSessions` therefore counts, for each of its players, the ticks since it last read one of its datagrams (`ConnectedPlayer::ticksSinceReceived`). Any datagram the game reads counts: an input, a `ReadyState`, or the `Heartbeat` a client sends after `HEARTBEAT_INTERVAL` (1 s) without sending anything else (see [Client](/R-Type/client/#joining-a-server-serverconnection)). When the count reaches `SESSION_TIMEOUT_TICKS` (300 ticks, the `SESSION_TIMEOUT` of 5 s), the player is **away**: `ConnectedPlayer::away` is set, its newest input is cleared so the ship stops, and the game logs `player <n> is away` at the `info` level under `Sessions`. The seat, the number, the session token, the ship, the score and the lives are kept, the ship stays in the world and can be hit, and `DatagramSorter` still admits the address, so the player is back with the first datagram it sends from the same address and port: the game clears `away` and restarts the count. An away player does not count in `ReadyKeeper::everyPlayerIsReady()`, so it does not hold back the start of the game, and it still holds its nickname. When the count reaches `SESSION_CLOSING_TICKS` (1200 ticks, the `SESSION_SILENCE_LIMIT` of 20 s: `SESSION_TIMEOUT` plus the `RECONNECTION_GRACE` of 15 s), the session closes in that tick, after the datagrams are read and before the game moves along its states:

1. the seat and the number are freed (`PlayerRoster::remove`), so a client killed and started again, which comes back from another port, takes a free seat instead of a fifth one; the steps below read the copy of the player the roster hands back;
2. the ship leaves the world (`World::removePlayer`) and, when the world held it, is published as an `EntityDestruction` (`Removed`), so the clients make it disappear without an explosion;
3. a `PlayerDeparture` (`TimedOut`) is published, which the others get as `PlayerLeft`, three times (see [Game events](#game-events-gameeventencoder-and-gameeventbroadcaster)). No client reads `PlayerLeft` yet: it skips it as a message without handler, so no player is told on screen (see [Protocol](/R-Type/protocol/#a-peer-disappears)).

`takeDepartedPlayers()` hands each departed player over once. The games module then calls `IPlayerAdmission::forget()`. The game events go to the players of the roster, so the next ones reach the others only (see [Game events](#game-events-gameeventencoder-and-gameeventbroadcaster)). The game logs `player <n> timed out: nothing received from <address> port <port> for 5 s` at the `info` level under `Sessions`.

- The game counts the ticks it runs, not the time that passes: during a stall of the server, the datagrams wait in the player queue and are read at the next tick, so a stall closes no session by itself. A stall long enough for the clients to take the server for lost closes each of their sessions all the same, `SESSION_CLOSING_TICKS` (1200) ticks after the last datagram the server read from its client (see [A long stall and the clients](#a-long-stall-and-the-clients)).
- The rest of the tick runs without the player: a waiting room whose other players are all ready starts the game in that tick, and the world state of that tick no longer lists the ship.
- A game in progress whose last player timed out or left ends in that tick, after the sessions close and before the game moves along its states, as `finish()` ends it: it takes no new player until the finished state is over, `FINISHED_STATE_TICKS` (300) ticks counted from that tick, then waits for players with a new world. The game logs `game finished: no player left` at the `info` level under `Instances`. A waiting room whose last player timed out keeps waiting.

A player who leaves does not wait for the timeout. Its client sends a `Disconnection` when the player leaves the waiting room or closes the window (see [Client](/R-Type/client/#joining-a-server-serverconnection)), and the session closes in the tick that reads it, through the same three steps, with a `PlayerDeparture` (`Left`): the others get `PlayerLeft` (`Left`) at once. The game logs `player <n> left from <address> port <port>` at the `info` level under `Sessions`. The two copies the client sends after the first are dropped: by the sorter once it forgot the address, or by the games module, which hands a datagram only to the game whose roster holds its sender, so no game reads them and no line is logged.

The server sends heartbeats too. At the end of each tick, a game sends a `Heartbeat` to each player that no datagram reached for `TICKS_PER_HEARTBEAT` (60) ticks, whoever sent the last one: every datagram to a player takes its header from `nextSessionHeader()`, which starts that count over. The waiting room (every 500 ms), a game in progress (20 states per second) and a game that ended by itself (`GameEnded` every 500 ms) already send more often, so only a game ended by `finish()` sends heartbeats.

`HEARTBEAT_INTERVAL`, `SESSION_TIMEOUT` and `RECONNECTION_GRACE` (`src/network/Protocol/ProtocolConstants.hpp`) are **provisional**, like every delay of the protocol. Code must read them, or their tick counts in `src/server/Sessions/SessionConstants.hpp`, and never repeat 1, 5, 15, 60, 300 or 1200.

### A long stall and the clients

A client takes the server for lost once `SESSION_SILENCE_LIMIT` (20 s, `SESSION_TIMEOUT` plus `RECONNECTION_GRACE`) passed since the last datagram it read, then sends nothing more (see [Client](/R-Type/client/#joining-a-server-serverconnection)). That datagram left before the stall began, so a stall somewhat shorter than `SESSION_SILENCE_LIMIT` can already be enough. The server then closes each of those sessions `SESSION_CLOSING_TICKS` (1200) ticks after the last datagram it read from that client.

## Waiting room: `LobbyStateAnnouncer`

`LobbyStateAnnouncer` (`src/server/Replication/LobbyStateAnnouncer/`) tells the players of a game who is in the waiting room and who is ready, as a `LobbyState` (see [Protocol](/R-Type/protocol/#a-session)). `GameFlow` calls `onWaitingTick()` on every tick it waits for players, and builds a new announcer with each new world, so the room goes out at the first tick of every wait.

- The room lists every connected player, in order of number, with its ready state and its nickname, read from the `PlayerRoster` at each tick.
- It is sent to every connected player as soon as it differs from the room sent last: in the tick a `ReadyState` changes it, and in the tick after a player joins, since the lobby module answers the connection requests after the games ticked. When that `ReadyState` makes every player ready, the game starts in the same tick and sends `GameStarted` instead.
- An unchanged room is sent again `TICKS_PER_LOBBY_STATE` (30) ticks after the last send. `LOBBY_STATE_INTERVAL` (500 ms) is **provisional**, like the protocol rate it follows; `TICKS_PER_LOBBY_STATE` is that interval in whole ticks, rounded down, so 30 ticks are 499.99998 ms, while adding up tick durations until 500 ms would wait 31 ticks, 516.7 ms. Code must read the constants (`src/server/ServerConstants.hpp`) and never repeat 500 or 30.
- Nothing is sent while the game is in progress or finished, and a room without players reaches no one.

## Player ships: `ShipSpawnAnnouncer`

A player gets a ship the moment the `ConnectionAcceptor` lets them in. `ShipSpawnAnnouncer` (`src/server/Replication/ShipSpawnAnnouncer/`) creates it with `World::spawnPlayer(position)` (`rtype_game`), which gives the entity a `Position`, a `CollisionBox` of `SHIP_WIDTH` by `SHIP_HEIGHT`, an `EntityType::Player` and a `NetworkIdentity` from the world's `NetworkIdAllocator`, and returns that network id. The id is kept in `ConnectedPlayer::shipEntityId` (`NO_ENTITY_ID` while the player has no ship).

`shipStartPosition(playerNumber, playerCount)` (`src/game/Spawning/`) says where the ship appears: at `SHIP_START_X_RATIO` (0.1) of the playfield width, so x is 192, and on a row of its own. The height is cut into `playerCount + 1` equal parts and player n takes the n-th cut, so with four players the rows are 216, 432, 648 and 864 logical units, each pair 216 apart. A number that is 0 or above `playerCount` throws `InvalidPlayerNumberException`.

The announcer then sends an `EntitySpawned` (`PlayerShip`, the position of the ship) in its own datagram, carrying the session token and the next sequence of the recipient:

- the new ship goes to every connected player, the new one included;
- the new player also gets one for each ship already in the game, so it sees the others. The position sent is the one the ship has in the world at that moment.
- A player who already has a ship gets no second one, and nothing is sent.
- A lost `EntitySpawned` is recovered from the world state (see [Protocol](/R-Type/protocol/#world-state)).
- When the session of a player closes, its ship leaves the world and the others get its `EntityDestroyed` (`Removed`) (see [Silent players](#silent-players-heartbeats-and-timeouts)).

```cpp
rtype::server::ShipSpawnAnnouncer shipSpawnAnnouncer{world, roster,
                                                     queueDatagram};
rtype::server::ConnectionAcceptor connectionAcceptor{
    roster, queueDatagram,
    [&admission](const rtype::network::Endpoint& player,
                 std::uint64_t sessionToken) {
      admission.admit(player, sessionToken);
    },
    [&shipSpawnAnnouncer](rtype::server::ConnectedPlayer& player) {
      shipSpawnAnnouncer.onPlayerJoined(player);
    }};
```

## Player inputs

Each game's `InputKeeper` keeps, in `ConnectedPlayer::newestInput`, the `PlayerInput` with the highest `inputNumber` each of its players sent, as the [Protocol](/R-Type/protocol/#inputs) requires. It reads them in `onTick`, through the same dispatcher and after the same checks as the `ReadyState` (see [A game](#a-game-gameinstance-and-gameflow)), so each datagram is decoded once. The field is empty until the first input arrives. Each tick of a game in progress applies it to the ship of the player (see [Moving the ships](#moving-the-ships-shipcontrol)) and, while it holds shoot, fires the missiles of that ship (see [Shooting](#shooting-missilelauncher)).

- An input that is older, a duplicate, or numbered like the one kept changes nothing, so a datagram that arrives late or twice never undoes a newer input. Each player is compared with its own inputs only. Inputs are ordered by their `inputNumber`, not by the sequence of their datagram.
- Inputs are kept in every state of the game, and the input kept outlives a new world: the client numbers its inputs for the whole session, so its next input stays newer. An input moves the ship only while the game is in progress: one kept in the waiting room moves the ship from the tick after the one that starts the game.
- The session token check of the sorter stops a sender that forges the address and port of a player: one forged input numbered 4294967295 would otherwise stay above every input the player sends for the rest of the session. It runs on the network thread, before the player queue, so a forged datagram takes no room in the queue either (see [Sorting the datagrams](#sorting-the-datagrams-datagramsorter)).
- `inputNumber` is compared as a plain number: at 60 inputs per second, it would take more than 800 days to wrap.
- A client that connects again from the same address and port gets back the same place and token, but numbers its inputs from 1 again: they would be ignored until they pass the input kept. No path of the client leads there today. A player whose session closed comes back as a new player, with no input kept.
- Each time the actions kept for a player change, the game logs `player <n> holds actions <byte>` at the `debug` level under `Sessions`.

## Moving the ships: `ShipControl`

`shipAfter(from, held, elapsed)` (`src/game/ShipControl/`, `rtype_game_ship_control`) says where a `MovingShip` is after `elapsed`, its player holding the `ShipDirections` `held` (up, down, left, right), and how it moves then. A `MovingShip` is a position and a `ShipMotion`, the component `World::spawnPlayer` gives every ship: its velocity along each axis, the maneuver it runs and the last press of each direction. A new world or a respawn creates a new ship, so it starts at rest. `shipAfter` is a free function, not a system: a ship has no `Velocity`, so `MovementSystem` never moves it, and the client can run the same function to predict its own ship, which it does not do yet.

- The ship drifts. Along each axis, its velocity chases the direction held times `SHIP_SPEED` (480 logical units per second): each tick closes the share 1 − e^(−rate × tick) of the gap, and never less than floor × tick, so the curve ends instead of creeping. While the axis is held, the rate is `DRIFT_RISE_RATE` (6 per second) and the floor `DRIFT_RISE_FLOOR` (960 units per second squared): a ship at rest reaches `SHIP_SPEED` on its 21st tick, 350 ms. Once let go, `DRIFT_FALL_RATE` (4.5) and `DRIFT_FALL_FLOOR` (720): a ship let go at `SHIP_SPEED` glides 85 units and stops on its 28th tick. A reversal chases the new target from the current speed, so the ship slides a little before it turns. x grows to the right and y downwards.
- Two directions on two axes aim at `SHIP_SPEED` divided by the square root of 2 on each axis, 339.4 units per second, so a diagonal is no faster than a straight line. Two opposite directions cancel out: up and down together aim at nothing on that axis.
- The ship stops where its side meets an edge of the play area, the playfield less the status band at its bottom (`PLAY_AREA_HEIGHT`, 984 units from the top edge): its center stays from 64 to 1856 in x and from 32 to 952 in y, half of `SHIP_WIDTH` (128) and of `SHIP_HEIGHT` (64) away from each edge, so the whole ship stays in sight. It loses its velocity along the axis an edge stopped, so it never pushes on once let go.
- `SHIP_WIDTH` and `SHIP_HEIGHT` are the size the client draws the ship at: 32 by 16 pixels of the entity sheet (`PLAYER_SHIP_FRAME`), each covering `BACKGROUND_PIXEL_SIZE` (4) logical units. A client test compares the two, so a new ship sprite comes with new constants.
- `SHIP_SPEED` and the drift constants are **provisional**: they will be tuned once the game is played on screen. Code must read the constants (`src/game/ShipControl/ShipControlConstants.hpp`) and never repeat their values.

### Roll and dash: `motionAfterInput`

`motionAfterInput(motion, change)` reads a new input of a player for its ship: the directions it held before, the ones it holds now, and its `inputNumber`. A direction pressed again within `DOUBLE_TAP_INPUTS` (13) inputs of its last press starts a maneuver, which `shipAfter` carries on top of the drift, its distance eased out (fast first, then slow):

| Pressed twice | Maneuver | Distance | Duration |
|---|---|---|---|
| down or up | roll | `ROLL_SIDESTEP` (112 units) that way | `ROLL_DURATION` (400 ms) |
| right | dash | `DASH_DISTANCE` (144 units) forward | `DASH_DURATION` (200 ms) |
| left | backstep | `BACKSTEP_DISTANCE` (96 units) back | `DASH_DURATION` |

The window is counted in input numbers, the clock of the client, which sends 60 inputs per second: 13 inputs are 217 ms, whatever the delays of the network. Nothing starts while a maneuver runs, a roll waits `ROLL_COOLDOWN` (500 ms) after the last roll, a dash or a backstep waits `DASH_COOLDOWN` (500 ms) after the last of them, so they are not chained back to back, and the press that started a maneuver is not the first press of the next one, so two slow taps stay two small moves. A rolling ship is hit like any other. These values are **provisional**, like `SHIP_SPEED`.

Each tick of a game in progress, `GameFlow` moves the ship of every connected player who sent an input by one `SIMULATION_TICK_DURATION` with `shipAfter`, before the systems run. It reads the directions from the `actions` byte of the input with the bit positions the protocol names in `ProtocolConstants.hpp` (`UP_ACTION_BIT_POSITION` to `SHOOT_ACTION_BIT_POSITION`), through `directionsOf` in `src/server/Instances/ShipInput/`; the shoot bit is not a direction. Every input the `InputKeeper` keeps is first shown to the `onNewerInput` the game gives its sessions, which runs `motionAfterInput` against the input kept before it (`noteTaps`, also in `ShipInput`), so a double press is read even when its inputs arrive in the same tick. A player who sent no input yet keeps its ship still, and the newest input keeps moving the ship until a newer one arrives. The ships do not move while the game waits for players or is finished. Each ship and its motion are found by network id with `World::shipPositionOf(shipId)` and `World::shipMotionOf(shipId)`, which look it up in a table of the ships of the world instead of visiting every entity, and give `nullptr` for any other id.

```cpp
rtype::game::MovingShip ship{.position = start, .motion = {}};
ship = rtype::game::shipAfter(
    ship, rtype::game::ShipDirections{.up = true, .right = true},
    rtype::engine::SIMULATION_TICK_DURATION);
```

## Game events: `GameEventEncoder` and `GameEventBroadcaster`

The game announces what the players must see or hear by publishing an event on the `EventBus`; the server turns it into a message and sends it to every client. The game publishes five of the events in `src/game/Events/` for the players: `EntityDestruction` (`Killed` or `Removed`), `EntityDamage`, `PlayerHit`, `PlayerElimination` and `PlayerDeparture` (`Left` or `TimedOut`). The broadcaster also reads `EntitySpawn` and the encoder can write it, but nothing publishes it: the ships, the Bydos and the missiles are announced by a direct send (`sendEntitySpawned`, `src/server/Replication/EntitySpawnSender.hpp`). They carry a network id or a player number, never an `Entity`: whoever publishes reads the id from the `NetworkIdentity` before the entity is destroyed.

`GameEventEncoder` (`src/server/Replication/GameEventEncoder/`) writes an event as the message of the protocol (see [Protocol](/R-Type/protocol/#game-events)) and says how many times to send it. An `EntitySpawn` becomes an `EntitySpawned`, sent once, because the next world state creates the entity anyway. An `EntityDestruction` becomes an `EntityDestroyed`, a `PlayerDeparture` a `PlayerLeft`, and the other three a `GameEvent` of kind `EntityDamaged`, `PlayerHit` or `PlayerEliminated`; these are sent `REPEATED_MESSAGE_COPIES` (3) times. The encoder numbers the `GameEvent`s from `FIRST_EVENT_NUMBER` (1), wrapping from 65535 to 0, and an event it refuses uses no number.

`GameEventBroadcaster` (`src/server/Replication/GameEventBroadcaster/`) subscribes to those events and the `EntitySpawn`, six in all, and sends each message to every recipient, in a datagram whose header comes from the function it was given for that recipient. The first copy leaves when the bus delivers the event; the others leave `REPEATED_MESSAGE_INTERVAL` (50 ms) and twice that later, through the `TimerScheduler`.

- The copies go to the clients there were when the bus delivered the event: a client that joins after the delivery never receives a copy.
- The timers advance by one tick at a time, so a copy leaves on the first tick that reaches its delay, up to one tick (`SIMULATION_TICK_DURATION`, 16.7 ms) after it.
- `REPEATED_MESSAGE_COPIES` and `REPEATED_MESSAGE_INTERVAL` are **provisional**, like the protocol rates they follow. Code must read the constants and never repeat 3 or 50.
- An event the protocol refuses is skipped: a position that is not a number, the entity id 0 (which no entity has), or a player number outside 1 to 4. The broadcaster logs a warning with the name of the event, sends nothing, schedules no copy, and does not use an event number, so the next event goes out and the server keeps running. Only the encoding is protected: a fault in the sending still stops the tick.
- The broadcaster keeps the handle of each of its subscriptions and gives them back to the bus when it is destroyed, which the replication module does when it stops, so the bus never calls it after that. It must outlive the last `advance()` of the timers, which calls back into it.
- Nothing is sent or scheduled when the list of recipients is empty, but the encoder still runs, so a `GameEvent` still uses an event number.
- In the games module the recipients are the players in the roster of the game when the bus delivers the event (`PlayerRoster::endpoints()`), and the header function is `sessionHeaderFor` on the same roster, as `GameInstance::nextSessionHeaderFor` does (see [Session headers](#session-headers-sessiondatagram)), so each copy carries the token and the next sequence of its player. A player whose session closes in a tick leaves the roster before the bus delivers the events of that tick, so the others alone get its departure. A copy of an earlier event still goes to its address, under a header without session, which its client drops.
- `PlayfieldExitSystem` publishes an `EntityDestruction` when a Bydo or a missile, of a player or of a Bydo, leaves the playfield. `DamageRules` publishes one for each missile that hits and for each Bydo it destroys, and an `EntityDamage` for a Bydo hit that survives. The players get each of them.

```cpp
rtype::engine::EventBus events;
rtype::engine::TimerScheduler timers;
rtype::server::GameEventBroadcaster broadcaster{
    rtype::server::GameEventBroadcaster::Wiring{
        .events = events,
        .timers = timers,
        .recipients = [&roster] { return roster.endpoints(); },
        .headerFor =
            [&roster](const rtype::network::Endpoint& recipient) {
              return rtype::server::sessionHeaderFor(roster, recipient);
            },
        .send = send}};

events.publish(rtype::game::PlayerHit{.playerNumber = 2});
timers.advance(rtype::engine::SIMULATION_TICK_DURATION);
events.dispatch();
```

## Bydos: `BydoSpawner`

`BydoSpawner` (`src/server/Spawning/BydoSpawner/`) makes the Bydos appear in the `World`, in waves: a `WaveDirector` says when (see [Waves of Bydos](#waves-of-bydos-wavedirector)). A `GameFlow` builds one with each world, from a `BydoSpawnerParts` holding the world, the players, the send function, the seed and the wave plan of the game, and ticks it only while the game is `InProgress`, so nothing appears while the game waits for players or is finished, and every new game starts its waves over with the first pause. With the `normal` difficulty, the first Bydo of a game appears on the 241st tick, 4 seconds in.

- `bydoSpawnPosition(randomUnit)` (`src/game/Spawning/`) says where: `BYDO_SPAWN_MARGIN` (64 logical units) past the right edge of the playfield, so the Bydo is off screen, and at a height from `BYDO_ROW_MARGIN` (112) to `PLAY_AREA_HEIGHT` (984) minus that margin, 872. The margin is `BYDO_SPAWN_VERTICAL_MARGIN` (64), more than half of the height of a Bydo, and the room a Bydo may move on its pattern, `BYDO_PATTERN_ROOM` (48), so even at the bottom of a wave a Bydo stays above the status band every client draws along the bottom of the playfield (see [Architecture](/R-Type/architecture/#coordinate-system)). A draw of 0 gives the highest row; a draw from 0 up to, but not including, 1 is accepted, anything else (1 included, or not a number) throws `InvalidRandomUnitException`. The margins are **provisional**: chosen before a Bydo had a size, they are wider than half of its 96 units, so a Bydo still appears and leaves off screen.
- `World::spawnBydo(position, kind)` gives the entity a `Position`, a `Velocity` of `-BYDO_SPEED` (240 units per second, **provisional**, to the left), a `MovementPattern`, `EntityType::Enemy`, `Health`, a `NetworkIdentity` and a `CollisionBox` of `BYDO_WIDTH` × `BYDO_HEIGHT` (96 × 96 logical units, its 24 × 24 pixels of the entity sheet; a client test compares the two). The spawner gives the Bydos the kinds the wave plan lists, in turn, the turn carrying on from one wave to the next (`Drifter` then `Weaver` with `normal`), and draws nothing for it, so a seed still gives the same heights. A plan without kinds throws `EmptyEnemyCycleException`. See [Bydo movement](#bydo-movement-patternsystem-and-playfieldexitsystem).
- Every connected player is sent an `EntitySpawned` (`Bydo`, the position), in its own datagram, like the ship of a player. Both use `sendEntitySpawned` (`src/server/Replication/EntitySpawnSender.hpp`). With no player connected, the Bydo appears and nothing is sent. When every network id was given, the spawn is skipped and a warning is logged.
- The generator is a `std::mt19937` started from a seed. The spawner logs it at the `info` level when it is built (`Bydo spawning starts from the seed 42`), so the same seed gives the same heights in the same order. `GameFlow` gives each world the seed of the game plus the number of worlds built before it, so one seed replays every game of a session; without `--seed`, the server draws it from `std::random_device`. The spawner also logs each wave at the start of the pause before it (`wave 2: 4 Bydos, one every 1750 ms`).

```bash
./r-type_server --seed 42
```

## Bydo movement: `PatternSystem` and `PlayfieldExitSystem`

Each tick of a game in progress, `GameFlow` moves the ships (see [Moving the ships](#moving-the-ships-shipcontrol)), then runs four systems of the world, in this order, before the missiles are fired and the Bydos appear: `PatternSystem`, `MovementSystem`, `PlayfieldExitSystem`, then `CollisionSystem` (see [Collisions](#collisions)). It runs them with `SIMULATION_TICK_DURATION`, and the scheduler belongs to the `GameInstance`, which gives it to the modules: `RtypeRulesModule` adds the four systems when it starts (`addRtypeSystems`), so they survive the new worlds.

- A Bydo moves left at `BYDO_SPEED` through its `Velocity`, which `MovementSystem` applies. Its `MovementPattern` holds a `PatternShape`, the row it appeared on (`originY`) and its age in seconds.
- `PatternSystem` (`src/game/Systems/PatternSystem/`) sets the height of every entity that has a `MovementPattern`. `Straight` keeps the row. `Wave` puts the entity at `originY + WAVE_AMPLITUDE * sin(2 * pi * age / WAVE_PERIOD_SECONDS)`, with an amplitude of 48 units and a period of 2 seconds, both **provisional**. The height is computed from the age, never added up tick by tick, so it does not depend on how the time is cut into steps. The amplitude stays within `BYDO_PATTERN_ROOM`, the room every row keeps above and below it, checked at compile time, so a Bydo on a wave stays in the area of play.
- `patternShapeOf(kind)` (`src/game/Spawning/BydoKind/`) says which shape a kind follows: the `Drifter` goes `Straight`, the `Weaver` follows a `Wave`.
- `PlayfieldExitSystem` (`src/game/Systems/PlayfieldExitSystem/`) destroys an entity of type `Enemy` that has a `NetworkIdentity` once its `Position` is more than `BYDO_EXIT_MARGIN` (64 logical units, **provisional**) beyond the left edge, and publishes an `EntityDestruction` with the cause `Removed` under its network id. A Bydo exactly at the margin is still in. It removes a missile of a player the same way once its center is more than half of `PLAYER_MISSILE_WIDTH` beyond the right edge (see [Shooting](#shooting-missilelauncher)), and a missile of a Bydo once its center is more than half of `ENEMY_MISSILE_WIDTH` beyond the left edge (see [Enemy fire](#enemy-fire-bydogunner)); a Bydo or a missile of a Bydo past the right edge, where Bydos appear, stays. A ship is never removed by leaving. It takes the `EventBus` of the server, which `GameInstance` receives at construction and which must outlive it.
- A Bydo crosses the playfield in about 8.5 seconds: 2048 logical units at `BYDO_SPEED`, from `BYDO_SPAWN_MARGIN` past the right edge to `BYDO_EXIT_MARGIN` past the left one.
- The players are sent an `EntityDestroyed` (`Removed`) for each Bydo removed, `REPEATED_MESSAGE_COPIES` (3) times (see [Game events](#game-events-gameeventencoder-and-gameeventbroadcaster)).

## Waves of Bydos: `WaveDirector`

`WaveDirector` (`src/game/Waves/WaveDirector/`, `rtype_game_wave_director`) decides when the Bydos of a game appear. It knows no world, no file and no network: each tick, `BydoSpawner` tells it how many entities of type `Enemy` the world holds, and it answers how many Bydos must appear now.

- The waves grow along a `WaveCurve` (`src/game/Waves/WaveCurve.hpp`): its first `BydoWave` (how many Bydos, and the time between two), the Bydos added and the time cut per wave, the shortest time between two Bydos and the pause before each wave. `bydoWaveAfter` (`src/game/Waves/WaveProgression/`) gives the wave after a wave. `isPlayable` is false for a curve with no Bydo, no time between two Bydos, waves that slow down or a negative duration, and the director throws `InvalidWaveCurveException` on such a curve.
- A game starts with a pause. The Bydos of a wave are due one by one, the first at the end of the pause, the next ones an interval apart, on the timers of a `TimerScheduler`, so a wave never drifts. Once every Bydo of a wave is due, the next wave waits for a tick that finds no `Enemy` in the world, killed or removed past the left edge, and its pause starts on that tick. A Bydo gone before the last of its wave is due does not end the wave.
- The spawner counts the enemies before it creates the Bydos due, so the tick on which the last Bydo of a wave appears never ends that wave. A Bydo `PlayfieldExitSystem` removes stops counting on the tick it leaves; a Bydo missiles destroy stops counting on the tick after the bus delivers its destruction.
- A duration ends on the tick that reaches it: a tick lasts 16 666 666 ns, so a pause of 4 seconds ends on its 241st tick.

## Waves file: `WaveCatalog`

`WaveCatalog` (`src/server/Spawning/WaveCatalog/`, `rtype_server_wave_catalog`) reads the waves of each difficulty from a TOML file with toml++. It is the only target that links toml++, and only the server links it, so no client target can read the levels. When the games module starts, it reads `WAVE_FILE_PATH` (`assets/waves/waves.toml`) from the folder the server is started in, takes the `WavePlan` of `DEFAULT_DIFFICULTY` (`normal`) and gives it to the game in its `GameSettings`, with the seed. A `WavePlan` holds a `WaveCurve` and the kinds its Bydos take in turn.

Every key, value and enemy name of every difficulty is checked. The first error throws a `ServerException` (`src/server/Exceptions/`) naming the file, the line and the column, and the dotted name of the key; `runServer` prints it and returns 1. The format, the ranges, the shipped difficulties and every error are in [Writing the waves file](/R-Type/waves-file/).

```cpp
const rtype::server::WaveCatalog catalog =
    rtype::server::WaveCatalog::fromFile(
        std::filesystem::path{rtype::server::WAVE_FILE_PATH});
rtype::server::WavePlan waves =
    catalog.planOf(rtype::server::DEFAULT_DIFFICULTY);
```

## Shooting: `MissileLauncher`

`MissileLauncher` (`src/server/Spawning/MissileLauncher/`) fires the missiles of the players. Each tick of a game in progress, after the systems run, `GameFlow` hands it the connected players whose newest input holds the shoot bit (`SHOOT_ACTION_BIT_POSITION`), and the ship of each fires one missile, unless it fired less than `SHOT_INTERVAL_TICKS` (20) ticks ago. Holding shoot therefore fires at once, then three times per second, `SHOTS_PER_SECOND` (3, **provisional**).

- The wait of each ship is counted in the ticks the game runs, whatever its player holds meanwhile, so releasing shoot and pressing it again fires no sooner. The game reads one input per player per tick, the newest, so sending more inputs fires no faster either: a modified client cannot beat the rate.
- `missileStartPosition(ship)` (`src/game/Spawning/`) puts the missile on the row of the ship, its back against the front of the ship: its center is 96 logical units ahead of the center of the ship, half of `SHIP_WIDTH` (128) plus half of `PLAYER_MISSILE_WIDTH` (64).
- `World::spawnPlayerMissile(position, playerNumber)` gives it a `Position`, a `Velocity` of `PLAYER_MISSILE_SPEED` (960 units per second to the right, **provisional**, twice `SHIP_SPEED`), a `CollisionBox` of `PLAYER_MISSILE_WIDTH` × `PLAYER_MISSILE_HEIGHT` (64 × 24, its 16 × 6 pixels of the entity sheet), `EntityType::PlayerMissile`, a `MissileOwner` naming the player, so a kill is credited to it, and a `NetworkIdentity`.
- Every connected player is sent an `EntitySpawned` (`PlayerMissile`) in its own datagram, like a Bydo.
- `PlayfieldExitSystem` removes the missile once its center is past 1952, half its width beyond the right edge, and announces it as `Removed`. A missile crosses the playfield in 2 seconds.
- A launcher is built with each world, so every ship may fire at the start of a game, and the ships fire only while the game is in progress. When every network id was given, no missile appears and a warning is logged.
- A player still holding shoot when a game ends fires at the first tick of the next game, until its client sends a new input, as the arrows keep the ship moving.

Code must read the constants (`src/server/Spawning/MissileLauncherConstants.hpp`, `src/game/Components/MissileConstants.hpp`, `src/game/Components/EntitySizeConstants.hpp`) and never repeat 3, 20, 960, 64 or 24.

## Enemy fire: `BydoGunner`

`BydoGunner` (`src/server/Spawning/BydoGunner/`) fires the missiles of the Bydos. Every Bydo carries a `Gun` (`src/game/Components/Gun.hpp`), which `World::spawnBydo` gives it with no tick counted. Each tick of a game in progress, after the missiles of the players are fired and before a Bydo may appear, `GameFlow` has the gunner count one tick on the gun of every Bydo; a gun that reaches `ENEMY_SHOT_INTERVAL_TICKS` (90) starts again from 0, and its Bydo fires. A Bydo therefore fires `ENEMY_SHOT_INTERVAL` (1.5 s, **provisional**) after it appears, then every 1.5 s, whatever its kind, and always straight to the left.

- `enemyMissileStartPosition(bydo)` (`src/game/Spawning/EnemyMissileStartPosition/`) puts the missile on the row of the Bydo, its right side against the left side of the Bydo: its center is 64 logical units before the center of the Bydo, half of `BYDO_WIDTH` (96) plus half of `ENEMY_MISSILE_WIDTH` (32).
- `World::spawnEnemyMissile(position)` gives it a `Position`, a `Velocity` of `-ENEMY_MISSILE_SPEED` (360 units per second to the left, **provisional**, three quarters of `SHIP_SPEED`, so a ship outruns it), a `CollisionBox` of `ENEMY_MISSILE_WIDTH` × `ENEMY_MISSILE_HEIGHT` (32 × 32, its 8 × 8 pixels of the entity sheet; a client test compares the two), `EntityType::EnemyMissile` and a `NetworkIdentity`. It has no `MissileOwner`, and it hits only ships (see [Rules](#rules-collisionrules)).
- Every connected player is sent an `EntitySpawned` (`EnemyMissile`) in its own datagram, like a Bydo; the client draws it with its own sprite, over everything of the playfield, and plays its orange muzzle flash (see [Client](/R-Type/client/#entity-sprites-spriteof-animationof)).
- `PlayfieldExitSystem` removes the missile once its center is past −16, half its width beyond the left edge, and announces it as `Removed`. A missile crosses the playfield in 5.3 seconds.
- A missile that hits a ship is removed by `DamageRules` and costs the player a life (see [Lives](#lives-lifekeeper)).
- With these values a Bydo fires five times on its way across: 1.5, 3, 4.5, 6 and 7.5 seconds after it appears, the last when the Bydo is at x 184, and it leaves at about 8.5 seconds. Each Bydo counts from its own appearance, so Bydos that appeared a multiple of 1.5 seconds apart fire on the same tick. In the `normal` difficulty, the five Bydos of the third wave, which come one every 1.5 seconds, fire together, and from the fifth wave on, which brings one every second, up to three fire on the same tick.
- A gunner is built with each world, and the Bydos fire only while the game is in progress. When every network id was given, the missile is skipped, a warning is logged, and the Bydo fires again an interval later.

Code must read the constants (`src/server/Spawning/BydoGunnerConstants.hpp`, `src/game/Components/MissileConstants.hpp`, `src/game/Components/EntitySizeConstants.hpp`) and never repeat 1.5 s, 90, 360 or 32.

## Collisions

A collision takes two steps, in two classes. The first finds what overlaps; the second decides which overlaps matter. Neither removes health nor destroys an entity: they only announce what happened, so the damage rules can react to it (see [Event bus](/R-Type/engine/#event-bus-eventbus)).

### Detection: `CollisionSystem`

`CollisionSystem` (`src/game/Systems/CollisionSystem/`, an `ISystem`) looks at every entity that has a `Position` and a `CollisionBox`. A `CollisionBox` is a size in logical units, centered on the `Position`. For each pair of boxes that overlap, it publishes one `Collision` (the engine event, see [Collisions](/R-Type/engine/#collisions-bounds-overlaps-and-collision)).

- Each overlapping pair is published once per update. Which of the two entities comes first carries no meaning.
- Boxes that only touch along an edge or at a corner do not overlap.
- An entity without a `Position` or without a `CollisionBox` is never reported.
- Every pair is tested, so the cost grows with the square of the number of boxes. The list of boxes is kept between updates, so a steady state allocates nothing.
- `update()` only queues the events. The loop must call `EventBus::dispatch()` after the systems, and run `CollisionSystem` after `MovementSystem`, so the boxes are tested where the entities now are.

### Rules: `CollisionRules`

`CollisionRules` (`src/game/Systems/CollisionRules/`) subscribes to `Collision` when it is created and unsubscribes when it is destroyed. For each `Collision` it reads the `EntityType` of both entities, in whichever order they come, and publishes a game event for three pairs only:

| Pair | Published |
|---|---|
| `PlayerMissile` and `Enemy` | `MissileHit{missile, target}`, the target being the enemy |
| `EnemyMissile` and `Player` | `MissileHit{missile, target}`, the target being the player |
| `Player` and `Enemy` | `ShipContact{player, enemy}` |

Every other pair is ignored: a player missile does not hurt a player, two enemies do not hurt each other, two missiles do not stop each other. An entity without an `EntityType`, or destroyed between the moment the `Collision` was published and the moment it is delivered, is ignored too.

The events are structs in `src/game/Events/`. `MissileHit` and `ShipContact` say that a contact happened; what it costs is decided by [`DamageRules`](#damage-damagerules) for a `MissileHit`, and by [`LifeKeeper`](#lives-lifekeeper) for a `ShipContact` and for a `MissileHit` whose target is a ship.

```cpp
rtype::engine::EventBus events;
rtype::game::CollisionRules rules{events, components};

scheduler.add(std::make_unique<rtype::game::MovementSystem>());
scheduler.add(std::make_unique<rtype::game::CollisionSystem>(events));

events.subscribe<rtype::game::MissileHit>([](const rtype::game::MissileHit& hit) {
  // remove health from hit.target, destroy hit.missile
});

scheduler.run(components, SIMULATION_TICK_DURATION);
events.dispatch();
```

The bus and the registry must outlive `CollisionRules`, and the bus must outlive `CollisionSystem`. `addRtypeSystems` adds `CollisionSystem` after `PlayfieldExitSystem`, and `GameFlow` builds a `CollisionRules` and a `DamageRules` with each world, on the registry of that world. The games module delivers the events in the `outputs` stage of every tick, so a collision is judged in the tick it is found, and never against the registry of a world replaced since. A ship has a `CollisionBox` from `World::spawnPlayer`, so a ship that touches a Bydo makes a `ShipContact`, and a missile of a Bydo that touches it a `MissileHit`.

## Damage: `DamageRules`

`DamageRules` (`src/game/Systems/DamageRules/`) decides what a `MissileHit` costs. It subscribes to `MissileHit` when it is created and unsubscribes when it is destroyed. For each hit, in this order:

1. The missile is destroyed and announced with an `EntityDestruction` whose cause is `Removed`, so the clients show nothing.
2. If the target has a `Health`, it loses `MISSILE_DAMAGE` (1) points. While it has points left it is announced with an `EntityDamage`. At 0 or less it is destroyed and announced with an `EntityDestruction` whose cause is `Killed`, which the client shows as an explosion.
3. A target without `Health` is not hurt: the missile still disappears.

- A Bydo has `Health` of `BYDO_HEALTH` (3) when `World::spawnBydo` creates it, so three missiles destroy it. Both values are **provisional** until gameplay tunes them. Code must read the constants and never assume 1 or 3.
- The ship of a player has no `Health`: a missile of a Bydo that hits it is removed here, and what that hit and a contact with a Bydo cost belongs to the lives (see [Lives](#lives-lifekeeper)).
- Network ids are read from the `NetworkIdentity` before the entity is destroyed. A hit naming an entity that is already gone, or that has no `NetworkIdentity`, is ignored, so two missiles that reach the same Bydo in one `dispatch()` kill it once, and the second missile is left alone.
- The bus and the registry must outlive `DamageRules`. It publishes while the bus delivers, so its events reach `GameEventBroadcaster` in the same tick.
- `GameFlow` builds it with each world (see [Collisions](#collisions)). A missile that hits therefore vanishes with no effect of its own; the clients play the hit of the Bydo on the `EntityDamaged` of the first two hits (see [Client](/R-Type/client/#effects-effectspawner)), and its explosion on the third.

```cpp
rtype::engine::EventBus events;
rtype::game::CollisionRules collisionRules{events, world.components()};
rtype::game::DamageRules damageRules{events, world.components()};
```

## Lives: `LifeKeeper`

`LifeKeeper` (`src/server/Instances/LifeKeeper/`) keeps the lives of the players of one world. It subscribes to `ShipContact` and `MissileHit` when it is created and unsubscribes when it is destroyed. A player joins with `STARTING_LIVES` (3), its ship included, and each new world gives them back.

1. A ship that touches a Bydo is destroyed, and so is the Bydo, each announced with an `EntityDestruction` (`Killed`), so the clients show two explosions. Nobody earns the points of the Bydo. A ship that a missile of a Bydo hits is destroyed the same way, announced `Killed`; `DamageRules` removes the missile.
2. The player loses a life, announced with a `PlayerHit`, then with a `PlayerElimination` when none is left.
3. A player with a life left has no ship for `RESPAWN_DELAY` (1 s, 60 ticks), then gets a new one at its start position through `IGameReplication::onPlayerJoined`, which announces it again.
4. That ship cannot be hit for `INVULNERABILITY_DURATION` (2 s, 120 ticks), the tick it comes back included: a Bydo it touches meanwhile costs nothing and stays, and a missile that hits it meanwhile disappears and costs nothing.

- A player with no life left has no ship and only watches: its input moves and fires nothing. Once no player left in the game has a life, the game ends (see [End of a game](#end-of-a-game-gameendannouncer)).
- A contact naming an entity already gone is ignored, so two Bydos touching one ship in the same `dispatch()` cost one life.
- The lives and the waits are kept in `ConnectedPlayer` (`lives`, `ticksUntilRespawn`, `invulnerableTicksLeft`). `onTick()` counts the waits down at each tick of a game in progress, and the world state sends `lives` and `invulnerable`.
- The three values are **provisional** until gameplay tunes them. Code must read the constants (`STARTING_LIVES` in `src/server/Sessions/ConnectedPlayerConstants.hpp`, the other two in `src/server/Instances/LifeKeeperConstants.hpp`) and never repeat 3, 1 s or 2 s.
- A hit on anything but the ship of a connected player, a Bydo hit by a missile of a player, changes nothing here.
- `DamageRules` must subscribe before the keeper. The bus hands each `MissileHit` to its subscribers in the order they subscribed, and `DamageRules` ignores a hit whose target is already gone, so in the other order the missile would fly on past the ship. `GameFlow::buildWorld()` builds them in that order, and a test of the game checks that the hit removes the missile.

```cpp
lifeKeeper_ = std::make_unique<LifeKeeper>(
    LifeKeeperParts{.events = events_.get(),
                    .world = world,
                    .roster = roster_.get(),
                    .respawn = [this](ConnectedPlayer& player) {
                      replication_.get().onPlayerJoined(player);
                    }});
```

`GameFlow` builds one with each world, in `buildWorld()`, and calls `onTick()` after the Bydos appear.

## End of a game: `GameEndAnnouncer`

A game in progress ends once no player left in it has a life. `LifeKeeper` takes the last one while the bus is dispatched, at the end of a tick; at the start of the next tick, after the sessions close, `GameFlow` finds no connected player with a life, logs `game finished: no player has a life left` at the `info` level under `Instances` and moves to `Finished`. A player who waits for its next ship still has a life, so the game goes on.

`GameEndAnnouncer` (`src/server/Replication/GameEndAnnouncer/`) then tells every player the result, as a `GameEnded` (see [Protocol](/R-Type/protocol/#a-session)):

- The result is read once, when the game ends: every connected player, in order of number, with its score. Every copy says the same, whoever leaves meanwhile.
- It goes to every connected player at the first tick of the end, one tick after the last life was lost, so after the first copy of the last explosion and of its game events. It goes again to the players still connected every `TICKS_PER_GAME_ENDED` (30) ticks, until the game waits for players again `FINISHED_STATE_TICKS` (300) ticks after the end.
- No world state goes out meanwhile. The new world then gives every player its `STARTING_LIVES` and a score of 0; the players stay connected and say ready to play again.
- `GAME_ENDED_INTERVAL` (500 ms) is **provisional**, like the protocol rate it follows; `TICKS_PER_GAME_ENDED` is that interval in whole ticks, rounded down. Code must read the constants (`src/server/ServerConstants.hpp`) and never repeat 500 or 30.

A game ended by `finish()`, or by the departure of its last player, tells no one a result.

## Where to intervene

| You want to… | Look at |
|---|---|
| Change what a tick does | The work a module adds to a stage: `playGames()` in `src/server/Modules/Games/ServerGamesModule.cpp`, `answerConnectionRequests()` in `src/server/Modules/Lobby/ServerLobbyModule.cpp` |
| Add a part to the server | A class implementing `IModule` in a folder of its own under `src/server/Modules/`, listed in `serverModules()` after the modules it depends on |
| Change what the server does when it stops | `ServerGamesModule::stop()` and `FarewellSender`, in `src/server/Modules/Games/` |
| Change which datagrams reach the tick | `DatagramSorter::sort` in `src/server/Application/DatagramSorter/DatagramSorter.cpp` |
| Change how many connection requests a tick answers | `CONNECTION_REQUEST_QUEUE_CAPACITY` in `src/server/ServerConstants.hpp` |
| Add a launch option | The options listed in `LaunchOptions::read()` and `usage()`, in `src/server/LaunchOptions/LaunchOptions.cpp`; a name the client uses too goes in `src/engine/LaunchArguments/LaunchArgumentsConstants.hpp` |
| Change the port the server listens on by default | `DEFAULT_SERVER_PORT` in `src/network/NetworkConstants.hpp`, shared with the client |
| Change how often the world state is sent | `WORLD_STATES_PER_SECOND` in `src/server/ServerConstants.hpp` |
| Change how often an unchanged waiting room is sent again | `LOBBY_STATE_INTERVAL` in `src/server/ServerConstants.hpp` |
| Change what the waiting room lists | `roomOf` in `src/server/Replication/LobbyStateAnnouncer/LobbyStateAnnouncer.cpp` |
| Change when a game ends | `reasonToFinish` in `src/server/Instances/GameFlow/GameFlow.cpp` |
| Change how often the end of a game is sent again | `GAME_ENDED_INTERVAL` in `src/server/ServerConstants.hpp` |
| Send a datagram to a player during a session | `sessionDatagram` in `src/server/Sessions/SessionDatagram/SessionDatagram.hpp`, then the send function of the game |
| Announce something to every client | Publish one of the events of `src/game/Events/` on the `EventBus`; `GameEventBroadcaster` sends it |
| Send a new kind of event | A struct in `src/game/Events/`, an `encode` overload in `GameEventEncoder`, a `subscribeTo` line in the `GameEventBroadcaster` constructor |
| Change how often an event is sent, or how far apart | `REPEATED_MESSAGE_COPIES` and `REPEATED_MESSAGE_INTERVAL` in `src/server/Replication/GameEventConstants.hpp` |
| Change the lives a player starts with, the wait for its new ship, or how long that ship cannot be hit | `STARTING_LIVES` in `src/server/Sessions/ConnectedPlayerConstants.hpp`, `RESPAWN_DELAY` and `INVULNERABILITY_DURATION` in `src/server/Instances/LifeKeeperConstants.hpp` |
| Change what a ship that touches an enemy, or that a missile of an enemy hits, costs | `LifeKeeper` in `src/server/Instances/LifeKeeper/` |
| Change the health of a Bydo, or the damage of a missile | `BYDO_HEALTH` in `src/game/Components/HealthConstants.hpp`, `MISSILE_DAMAGE` in `src/game/Systems/DamageRules/DamageConstants.hpp` |
| Change what a hit does | `DamageRules::onMissileHit` in `src/game/Systems/DamageRules/DamageRules.cpp` |
| Change what the world state lists | `WorldStateSender::capture` in `src/server/Replication/WorldStateSender/WorldStateSender.cpp` |
| Send a datagram from the tick | `ServerApplication::queueDatagram()` |
| Change how a connection request is answered | `ConnectionAcceptor::answer` in `src/server/Lobby/ConnectionAcceptor/ConnectionAcceptor.cpp` |
| Change where a ship appears | `shipStartPosition` and `SHIP_START_X_RATIO` in `src/game/Spawning/` |
| Change how fast a ship moves, drifts, rolls or dashes, or its size | `SHIP_SPEED`, the `DRIFT_`, `ROLL_` and `DASH_` constants, `DOUBLE_TAP_INPUTS`, `SHIP_WIDTH` and `SHIP_HEIGHT` in `src/game/ShipControl/ShipControlConstants.hpp`; a new size goes with a new `PLAYER_SHIP_FRAME` on the client, which a test compares |
| Change how a ship moves from the directions held, or what a double press starts | `shipAfter` and `motionAfterInput` in `src/game/ShipControl/ShipControl.cpp` |
| Change which actions of an input move the ship | `directionsOf` in `src/server/Instances/ShipInput/ShipInput.cpp`, with the bit positions of `src/network/Protocol/ProtocolConstants.hpp` |
| Change how many Bydos come, how often, or which kinds | `assets/waves/waves.toml`; see [Writing the waves file](/R-Type/waves-file/) |
| Change how a wave follows the one before | `bydoWaveAfter` in `src/game/Waves/WaveProgression/WaveProgression.cpp` |
| Change when the next wave starts | `WaveDirector::advance` in `src/game/Waves/WaveDirector/WaveDirector.cpp` |
| Add a key to the waves file, or a kind of enemy the file can name | the keys, ranges and `ENEMY_NAMES` in `src/server/Spawning/WaveFileConstants.hpp`, read by `src/server/Spawning/WaveCatalog/WaveCatalog.cpp` |
| Change which difficulty the games play, or where the server finds the waves file | `DEFAULT_DIFFICULTY` and `WAVE_FILE_PATH` in `src/server/ServerConstants.hpp` |
| Change where a Bydo appears | `bydoSpawnPosition` and the margins in `src/game/Spawning/` |
| Change how fast a Bydo moves, or the height and period of a wave | `BYDO_SPEED`, `WAVE_AMPLITUDE`, `WAVE_PERIOD_SECONDS` in `src/game/Systems/PatternSystem/PatternConstants.hpp` |
| Change which pattern a kind of Bydo follows | `patternShapeOf` in `src/game/Spawning/BydoKind/BydoKind.cpp` |
| Change when a Bydo or a missile leaves the playfield | `BYDO_EXIT_MARGIN` in `src/game/Spawning/SpawningConstants.hpp`, `hasLeftThePlayfield` in `src/game/Systems/PlayfieldExitSystem/PlayfieldExitSystem.cpp` |
| Change how often a ship fires | `SHOTS_PER_SECOND` in `src/server/Spawning/MissileLauncherConstants.hpp` |
| Change which action fires | `shootersIn` in `src/server/Instances/GameFlow/GameFlow.cpp` |
| Change where a missile appears, how fast it flies or its size | `missileStartPosition` in `src/game/Spawning/`, `PLAYER_MISSILE_SPEED` in `src/game/Components/MissileConstants.hpp`, `PLAYER_MISSILE_WIDTH` and `PLAYER_MISSILE_HEIGHT` in `src/game/Components/EntitySizeConstants.hpp`; a new size goes with a new `PLAYER_MISSILE_FRAME` on the client, which a test compares |
| Change how often a Bydo fires | `ENEMY_SHOT_INTERVAL` in `src/server/Spawning/BydoGunnerConstants.hpp`; which Bydos fire, `BydoGunner` in `src/server/Spawning/BydoGunner/` |
| Change where a missile of a Bydo appears, how fast it flies or its size | `enemyMissileStartPosition` in `src/game/Spawning/EnemyMissileStartPosition/`, `ENEMY_MISSILE_SPEED` in `src/game/Components/MissileConstants.hpp`, `ENEMY_MISSILE_WIDTH` and `ENEMY_MISSILE_HEIGHT` in `src/game/Components/EntitySizeConstants.hpp`; a new size goes with a new `ENEMY_MISSILE_FRAME` on the client, which a test compares |
| Change the size a Bydo is hit at | `BYDO_WIDTH` and `BYDO_HEIGHT` in `src/game/Components/EntitySizeConstants.hpp`, with `BYDO_FRAME` on the client, which a test compares |
| Replay a game | Start the server with the seed found in its log: `--seed <seed>` |
| Change who is told about a new ship | `ShipSpawnAnnouncer` in `src/server/Replication/ShipSpawnAnnouncer/` |
| Change when a game starts or ends, what the players are told at the start, or what is reset between two games | `GameFlow::advanceState`, `announceGameStart`, `startNewWorld` and `buildWorld` in `src/server/Instances/GameFlow/GameFlow.cpp` |
| Change how long a game stays finished | `FINISHED_STATE_SECONDS` in `src/server/Instances/GameInstanceConstants.hpp` |
| Change how players get their number, or what the server keeps of them | `PlayerRoster` in `src/server/Sessions/PlayerRoster/`, `ConnectedPlayer` in `src/server/Sessions/ConnectedPlayer.hpp` |
| Read the input a player holds | `ConnectedPlayer::newestInput`, kept by the `InputKeeper` of its game |
| Change which input of a player the server keeps | `isNewer` in `src/server/Sessions/InputKeeper/InputKeeper.cpp` |
| Change which datagrams of the players a game reads | `DatagramReader::read` in `src/server/Sessions/DatagramReader/DatagramReader.cpp` |
| Change how long a game waits for a silent player, how long it keeps its seat, or how often a side sends a heartbeat | `SESSION_TIMEOUT`, `RECONNECTION_GRACE` and `HEARTBEAT_INTERVAL` in `src/network/Protocol/ProtocolConstants.hpp`, shared with the client |
| Change what happens when the session of a player closes | `Departures::close` in `src/server/Sessions/Departures/Departures.cpp`, `RtypeRulesModule::removeShipOf` in `src/server/Instances/RtypeRulesModule/RtypeRulesModule.cpp`, and `playGames()` in `src/server/Modules/Games/ServerGamesModule.cpp` |
| Change the size of the queues between threads | `INCOMING_DATAGRAM_QUEUE_CAPACITY` and `OUTGOING_DATAGRAM_QUEUE_CAPACITY` in `src/server/ServerConstants.hpp` |
| Change the simulation rate | `SIMULATION_TICKS_PER_SECOND` in `src/engine/Time/TimeConstants.hpp` |
| Change how often the simulation reports | `REPORT_SECONDS` in `src/server/ServerConstants.hpp` |
| Change how many ticks the server catches up after a stall | `MAXIMUM_TICKS_PER_ADVANCE` in `src/engine/Time/TimeConstants.hpp`, read by every `FixedTimestep` |
| Write code that differs between Windows and the other systems | `src/server/Platform/`: a `Windows.cpp` file and a `Posix.cpp` file per class, chosen in its `CMakeLists.txt` |
| Test code that uses a socket without the network | `tests/doubles/RecordingDatagramSocket.hpp` |
| Test code that receives datagrams late, twice or never | `tests/doubles/UnreliableNetwork.hpp` |
| Test the whole server | `tests/server/Application/ServerApplicationTest.cpp`: a client on the loopback address, and a tick handler that records or throws |
| Change which pairs of entities matter | `CollisionRules::publishIfRuleMatches` in `src/game/Systems/CollisionRules/CollisionRules.cpp` |
| React to a hit or a contact | `subscribe<MissileHit>` or `subscribe<ShipContact>` on the `EventBus`; the structs are in `src/game/Events/` |
| Change when two boxes count as overlapping | `overlaps` in `src/engine/Collision/Overlaps.hpp` |
| Change how entities move | `MovementSystem` in `src/game/Systems/MovementSystem/`; the ships move through `ShipControl` in `src/game/ShipControl/` instead |
| Add a component type | A new struct in `src/game/Components/` |
| Write a test with entities that collide | `tests/CollisionScene.hpp` |

Each folder holding sources has its own `CMakeLists.txt` declaring one library; `Application/` and `Platform/` declare the INTERFACE library of their header-only files (`rtype_server_application_interfaces`, `rtype_server_platform_interfaces`) and add their subfolders, `Modules/` declares the constants of the modules and adds one folder per module (`Logger/`, `Network/`, `Games/`, `Lobby/`, each a module in the flat layout of [Project layout](/R-Type/project-layout/#adding-a-class) with one library, `rtype_server_<name>_module`) and `Assembly/`, which holds the list of modules (`rtype_server_modules`), `Replication/` declares its constants and adds one folder per sender (`WorldStateSender/`, `LobbyStateAnnouncer/`, `ShipSpawnAnnouncer/`, the event encoder and broadcaster…), `Lobby/` declares its constants and adds `ConnectionAcceptor/`, `Sessions/` declares the include folder and adds `ConnectedPlayers/`, `PlayerRoster/`, `SessionDatagram/` and one folder per class of the sessions of a game, `Instances/` declares the contracts of a game (`GameState`, `GameSettings`, `GameContext`, `GameInstanceParts`, `IGameModule`, `IGameFlow`, `IGameReplication`, `SessionHooks`) and the game constants and adds one folder per class (`WorldSlot/`, `GameFlow/`, `GameModules/`, `GameReplication/`, `ReplicationModule/`, `RtypeRulesModule/`, `GameInstance/`…), `Spawning/` declares the spawner parts, the wave plan and the waves file constants and adds `BydoSpawner/`, `WaveCatalog/` and `MissileLauncher/`, `Exceptions/` declares the parts of the server errors and adds `ServerException/`, and `r-type_server` links `rtype_server_application`, `rtype_server_connection_acceptor`, `rtype_server_game_event_broadcaster`, `rtype_server_game_instance`, `rtype_server_launch_options` and `rtype_server_wave_catalog` in `src/server/CMakeLists.txt`.
