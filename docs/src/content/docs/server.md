---
title: Server and game
description: What the server program and the game library do, how their parts fit together and where to find them in the code.
---

The server, `r-type_server`, is meant to run the game for the players connected to it. Today it receives their datagrams, runs a tick at a fixed rate, lets up to four players in and keeps one game that starts when they are all ready: each tick answers the connection requests received and reads the `ReadyState` and the inputs of the players. The game tells its players the waiting room, the start of the game and the world state, each datagram under the session token and the next sequence of its player. Nothing else a client sends is read yet, and nothing applies the inputs to the ships yet. It never links SFML or Asio: it reaches the network through `rtype_network`, and its own libraries are checked for it at configure time (see [Project layout](/R-Type/project-layout/#who-may-link-what)).

The game library, `rtype_game`, holds the rules of a match: what moves, what touches what and what that means. It is built on the engine (see [Engine](/R-Type/engine/)), and it knows nothing about sockets, SFML or the messages sent between the programs (see [Project layout](/R-Type/project-layout/#game)). The server runs all of it; the client will only run the part that predicts its own ship.

This page tells you where to look when you need to change something. It describes what exists today and says plainly what does not exist yet.

| Part | State | Where |
|---|---|---|
| Network thread and simulation thread | Available | `src/server/Application/` |
| Timer resolution on Windows | Available | `src/server/Platform/` |
| Sorting the datagrams before the queues, session token checked | Available; no limit per address yet | `src/server/Application/DatagramSorter/` |
| World state sent at a fixed rate to the players of the game | Available | `src/server/Replication/WorldStateSender/`, `src/server/Instances/GameInstance/` |
| One header for every datagram to a player: session token and next sequence | Available | `src/server/Replication/SessionDatagram.hpp` |
| Game events sent to the players | Available: the only event a running system publishes is the removal of a Bydo that leaves the playfield | `src/server/Replication/`, `src/game/Events/` |
| Launch options | Available: the port, the seed and `--help` | `src/server/LaunchOptions/` |
| Letting players in: challenge, acceptance or refusal | Available; refuses with `GameInProgress` while the game is not waiting for players | `src/server/Lobby/`, `src/server/Instances/` |
| A game: states, ready rule, new world between two games | Available: one game, nothing ends it yet | `src/server/Instances/GameInstance/` |
| Newest input of each player | Available; nothing applies it to the ship yet | `src/server/Instances/GameInstance/` |
| Waiting room and start of the game sent to the players (`LobbyState`, `GameStarted`) | Available | `src/server/Replication/LobbyStateAnnouncer/`, `src/server/Instances/GameInstance/` |
| End of a game sent to the players (`GameEnded`), several games, master link | Not yet written | — |
| Components (`Position`, `Velocity`, `CollisionBox`, `Health`, `EntityType`, `Sprite`, `Layer`) | Available | `src/game/Components/` |
| Movement, run by each game tick | Available | `src/game/Systems/MovementSystem/`, `src/server/Instances/GameInstance/` |
| Movement patterns, and removal of what leaves the playfield | Available for the Bydos | `src/game/Systems/PatternSystem/`, `src/game/Systems/PlayfieldExitSystem/` |
| Collisions | Available: nothing runs them yet | `src/game/Systems/CollisionSystem/`, `src/game/Systems/CollisionRules/`, `src/game/Events/` |
| Damage and destruction | Available: nothing runs it yet; players take no damage yet | `src/game/Systems/DamageRules/` |
| Ship of each player, created when the player joins and announced to the others | Available | `src/game/Spawning/`, `src/server/Replication/ShipSpawnAnnouncer/` |
| Bydos appearing off screen at random heights, on a timer, then moving left | Available | `src/game/Spawning/BydoSpawnPosition.*`, `src/server/Spawning/` |
| Waves, score, lives | Not yet written | — |

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

The tick handler is where the game plugs in. Today `src/server/main.cpp` gives one that gives the datagrams of the players to each game of its list, which reads the `ReadyState` and the inputs among them, moves along its states and tells its players the room, the start and the world state (see [A game](#a-game-gameinstance) and [Player inputs](#player-inputs)), hands the connection requests to the `ConnectionAcceptor`, which answers them and gives each new player a ship in the first game (see [Letting players in](#letting-players-in-connectionacceptor-playerroster) and [Player ships](#player-ships-shipspawnannouncer)), advances the timers by one tick, then delivers the events of the `EventBus`, which the broadcaster sends to the players who joined (see [Game events](#game-events-gameeventencoder-and-gameeventbroadcaster)).

A tick handler puts a datagram in the outgoing queue with `queueDatagram()`, called from the handler itself, on the simulation thread. The datagram is sent at the end of the same tick.

The sorter only lets through datagrams of this protocol whose message sizes add up, but anyone can still send a connection request, and the payloads are read by the tick, so the tick handler must drop and count a datagram it cannot decode, and never let an exception caused by a datagram's content escape: an exception stops the server (see below), so one truncated datagram would stop it for every player. `ByteReader`, for instance, throws `BufferUnderflowException` when a datagram is shorter than what it reads.

The player queue and the outgoing queue hold `INCOMING_DATAGRAM_QUEUE_CAPACITY` and `OUTGOING_DATAGRAM_QUEUE_CAPACITY` (256) datagrams each, about one second of the inputs of four players; the request queue holds `CONNECTION_REQUEST_QUEUE_CAPACITY` (16) (`src/server/ServerConstants.hpp`). These sizes are **provisional**: they will be revised with the rates of the protocol. Code must read the constants.

- When more datagrams arrive between two ticks than the player queue or the request queue holds, the oldest are discarded. Every `TICKS_PER_REPORT` ticks (600, so every ten seconds), the server logs one warning per queue with how many were discarded, if any, one with how many datagrams the sorter dropped before any queue, and one with how many it dropped for their session token, naming the address and port of the first of them (see below). Each datagram the sorter drops is also logged with its sender at the `debug` level (see below).
- A flood of connection requests, or of datagrams from strangers, never pushes a player's datagram out of its queue. It still delays the requests of new players: nothing limits the requests of one address yet (see [Flood protection](#flood-protection)).
- The outgoing queue also discards its oldest datagram when full, but nothing reports it yet.

`stop()` makes `run()` return, from any thread: the network thread stops at once, the simulation thread when its current sleep ends, about one tick later. An exception thrown on either thread, by the tick handler for instance, stops both, and `run()` rethrows it.

### Sorting the datagrams: `DatagramSorter`

`DatagramSorter` (`src/server/Application/DatagramSorter/`) decides, on the network thread, which queue each received datagram goes to, before any queue holds it. It reads the header and the type of each message, never a payload. The first row that applies decides where a datagram goes:

| The datagram… | Goes to |
|---|---|
| is not of this protocol and version, has a flag set, or its message sizes do not add up | nowhere: dropped |
| comes from the address and port of an admitted player without the session token of that player (a `ConnectionRequest` may carry 0 instead) | nowhere: dropped, and counted apart |
| holds a `ConnectionRequest`, whoever sent it | the request queue, where the `ConnectionAcceptor` answers it (see [Letting players in](#letting-players-in-connectionacceptor-playerroster)) |
| comes from an admitted player | the player queue |
| anything else | nowhere: dropped |

- A player is admitted by `ServerApplication::admit(endpoint, sessionToken)`, which the acceptor calls once for each sender it gives a place to, with the token it drew for that player; admitting an address again replaces its token. It may be called from any thread: it posts the admission to the network thread, so a datagram the player sends before that is dropped like a stranger's. No player is ever forgotten yet: nobody leaves the game.
- The `PlayerRoster` and the `ConnectionAcceptor` run in the tick, on the simulation thread; the network thread only keeps the address, port and session token of each admitted player.
- Each datagram the sorter drops is counted, readable from any thread: `wrongTokenCount()` for those dropped for their session token, `droppedCount()` for the others. The server reports both every ten seconds with the discards of its queues. The line about the tokens names the address and port of the first such datagram since the previous line (`takeWrongTokenSender()`): the address of a player, used by someone who does not know its token. A flood of forged datagrams therefore adds one line every ten seconds, never one per datagram. Besides these counts, every drop, a wrong token included, is logged at the `debug` level with its sender and why, such as `it is shorter than the packet header` or `its sender was not admitted, and it holds no connection request`, at most once per `REJECTION_LINE_INTERVAL` (10 s), with how many since the previous line (see [Logging](/R-Type/logging/#repeated-events-throttledlog)). The time of these lines comes from the `IClock` the server application gives the sorter, read only to drop a datagram.
- Measured on an Apple Silicon laptop (Apple M4 Pro, `-O2`, median of 7 runs of 2 million datagrams): sorting a datagram costs about 30 ns when a stranger's is dropped, 29 ns when one is dropped for its session token, the sender kept included, 18 ns for bytes shorter than a header, and 37 ns when it is queued, copy included. Reading the clock and counting the drop for its log line take 8 to 16 ns of a drop. A flood of 3 800 datagrams per tick therefore costs the network thread under 0.15 ms of every 16.7 ms, whatever the datagrams hold. A malformed datagram is found without throwing (`DatagramCodec::messageTypes`): an exception costs about 4 µs, so a flood of malformed datagrams would otherwise take about 14 ms of every tick. Answering a request costs about 0.45 µs, or 0.55 µs with a wrong cookie; the request queue keeps that under 9 µs per tick.

### Flood protection

The room of the player queue protects the server's memory; the sorter protects the players from the senders that were not admitted: one of them flooding the server's port with connection requests or with anything else no longer pushes the players' datagrams out. Someone who forges the address and port of an admitted player no longer can either, unless they read its session token on the way: the sorter drops their datagrams before the queue. That player itself, flooding with its own token, still can: the players share one queue (layer 2 below). A plain Python loop on the loopback address sends about 3 800 datagrams per tick (measured on an Apple Silicon laptop), about fifteen times the room of the queue. Other engines protect the players in three layers; the server must have them once sessions exist, and the first and the third are written, in part:

1. **Sort on the network thread, before the queue.** Written: `DatagramSorter` drops there every datagram that is neither from an admitted player nor a `ConnectionRequest`, and every datagram from the address of a player without its session token; the requests have their own queue. Not written: a limit on the requests of each address, in a table of fixed size so that forged addresses cannot make it grow. Quake 3 ignores such datagrams and answers at most 10 status, information or remote console requests per second from one address; Unreal limits how many datagrams of senders without a connection it processes each frame.
2. **One queue per session**, so a flooding client only fills its own. Valve's GameNetworkingSockets keeps one per connection, of 1 000 messages by default.
3. **One input per player and per tick.** The protocol requires it (see [Protocol](/R-Type/protocol/#inputs)). Written: each `GameInstance` keeps only the newest `PlayerInput` of each of its players and ignores older ones (see [Player inputs](#player-inputs)). Not written: applying it at every tick.

The player queue stays as the last safety net. It discards its oldest datagram, where GameNetworkingSockets refuses the newest, because only the newest input of a player counts. Why threads only talk through queues is explained in [Architecture](/R-Type/architecture/#queues-between-threads).

## Tick loop: `TickLoop`

`TickLoop` (`src/server/Application/TickLoop/`) calls a function once per tick on the thread that runs it. It counts ticks with a `FixedTimestep` and sleeps until its `nextTickTime()`, so the average rate stays at 60 ticks per second: after a late wake-up the missed ticks run back to back, and a stall longer than `MAXIMUM_TICKS_PER_ADVANCE` (5) ticks drops the excess. `ServerApplication` creates it when `run()` starts, so the first tick is due one tick after the call.

The limit is a choice for this game: during a longer stall the game freezes, then resumes where it stopped. Catching the whole stall up would play it in an instant, a second of enemy fire that nobody saw coming. Engines differ on it: a Quake 3 dedicated server catches up to 5 seconds, so that the clients' view of the server's clock stays right; Unity lets each game set its `maximumDeltaTime`.

Every `TICKS_PER_REPORT` ticks, the loop logs at the `debug` level how late its ticks started, each compared with the moment it was due; a dropped stall shows in full. The journal is `r-type_server.log`; `--log-stderr` also prints it in the terminal (see [Logging](/R-Type/logging/#choosing-the-level-and-the-output-at-launch)):

```bash
./r-type_server --log-level debug --log-stderr
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

`WorldStateSender` (`src/server/Replication/WorldStateSender/`) sends the state of the game to its players, at a fixed rate. Each `GameInstance` builds one with its `PlayerRoster` and calls `onTick(tick, components)` on the tick that starts the game and on every tick of the game, on the simulation thread, with the tick the game counts (see [A game](#a-game-gameinstance)). Every `TICKS_PER_WORLD_STATE` (3) ticks, that is `WORLD_STATES_PER_SECOND` (20) times per second, it lists every entity that has a `NetworkIdentity`, a `Position` and an `EntityType`, has `WorldStateCodec` encode the list (see [Network](/R-Type/network/#world-state-worldstatecodec)), and hands one `OutgoingDatagram` per connected player and per payload to the function it was given, each starting with the header of that player (see [Session headers](#session-headers-sessiondatagram)). In `main.cpp` that function is `queueDatagram()`.

- The `tick` of a state is the tick given to `onTick`, so the client can tell which of two states is newer. The first state of a game carries tick 0 and leaves in the tick that starts the game, right after `GameStarted`. The game's counter is 32 bits and is not made to wrap: it takes 2.27 years at 60 ticks per second. It starts again at 0 with each game.
- `WORLD_STATES_PER_SECOND` is **provisional**: it will be revised once measured on a real network. It must divide the tick rate, and the build fails otherwise. Code must read the constants, `TICKS_PER_WORLD_STATE` for instance, and never repeat the 3.
- Nothing is built when no player is connected. An entity without a network id, or without a type, is not listed.
- An entity whose position is not a finite number (NaN or infinite), which only a game bug can produce, is left out of the state instead of stopping the server. A warning under the `Replication` module gives how many entities were left out, each time that number changes, so a stuck entity is reported once rather than 20 times per second.
- The state lists no player yet: the game has no lives, score or player numbers. The ship of each player has a `NetworkIdentity` (see [Player ships](#player-ships-shipspawnannouncer)), so it is listed.
- A state split into several datagrams takes one sequence per datagram, so the client reads every part.
- A playfield larger than 2047 logical units in either direction fails the build, because a wire coordinate would no longer hold it (see [Protocol](/R-Type/protocol/#positions)).

```cpp
rtype::server::WorldStateSender worldStateSender{roster, send};
worldStateSender.onTick(tick, world.components());
```

`GameInstance` builds the sender once, with its roster, which lives as long as the game, and hands it the components of the current world at each call: a new world replaces the old one between two games.

`World::components()` (`rtype_game`) is the `ComponentRegistry` of the world: a `World` owns its entities and their components, and cannot be copied or moved.

## Letting players in: `ConnectionAcceptor`, `PlayerRoster`

`ConnectionAcceptor` (`src/server/Lobby/ConnectionAcceptor/`) answers the connection requests the sorter set aside for each tick, at most `CONNECTION_REQUEST_QUEUE_CAPACITY` (16), as the [Protocol](/R-Type/protocol/#connection) describes, and `PlayerRoster` (`src/server/Instances/PlayerRoster/`) holds the players it let in: at most `MAXIMUM_PLAYERS` (4), each a `ConnectedPlayer` with its number, nickname, address and port, session token, next sequence, ship id and newest input. A new player takes the lowest free number, so the first four are 1 to 4 in the order they joined.

A datagram is read only when its header is of this protocol and version and its message sizes add up; only its first `ConnectionRequest` is answered, so one datagram never gets more than one answer, and every other message is ignored. A request is answered by the first row that applies:

| The request comes… | Answer |
|---|---|
| from a connected player | the same `ConnectionAccepted` again (same number, same token) with a valid cookie; a `ConnectionChallenge` without one, even during a game |
| while a game is in progress | `ConnectionRefused` (`GameInProgress`) |
| when four players are connected | `ConnectionRefused` (`GameFull`) |
| without a valid cookie | `ConnectionChallenge`, and nothing is kept |
| with a valid cookie | a place in the roster, a session token drawn at random (never 0), and `ConnectionAccepted` |

- A refusal reserves nothing, so it goes before the cookie is checked. The cookies come from a `CookieIssuer` whose key is drawn when the acceptor is built (see [Network](/R-Type/network/#connection-cookies-cookieissuer)).
- An acceptance carries the session token and the next sequence of that player, 0 for the first (see [Session headers](#session-headers-sessiondatagram)). Challenges and refusals carry token 0 and sequence 0, as every datagram sent before a session.
- Besides the function that admits a sender, the acceptor takes an optional one it calls once for each player it lets in, after it sent the acceptance, and not again when an acceptance is repeated. `main.cpp` gives it the first `GameInstance`, which gives the player a ship.
- Nothing caused by the content of a datagram makes it throw. Each new player is logged at the `info` level under `Lobby`, as `player <n> joined from <address> port <port>`; challenges and refusals at the `debug` level. A datagram it cannot read, or whose request its reader refuses (a nickname empty, longer than 16 bytes, not UTF-8 or holding a control character, a cookie cut short), gets no answer and is logged at the `debug` level with its sender and why, at most once per `REJECTION_LINE_INTERVAL` (10 s) of the time the tick hands it.
- Each new player is announced once to the function given to the acceptor, which `main.cpp` hands to `ServerApplication::admit()`, so the datagrams of that player reach the tick from then on.
- `setGameInProgress(bool)` turns the `GameInProgress` refusal on and off. The tick handler sets it before the requests are read, to whether the first game is in any state other than waiting for players, so a request that arrives in the tick where the game starts is already refused. No player ever leaves the roster yet: heartbeats and departures do not exist.

## Session headers: `SessionDatagram`

Every datagram the server sends a connected player starts with the session token and the next sequence of that player, whatever its message, because the client drops a datagram that carries another token or that is not newer than the newest one it read (see [Protocol](/R-Type/protocol/#datagram)). The free functions of `SessionDatagram.hpp` (`src/server/Replication/`) are the one place that reads and moves `ConnectedPlayer::nextSequence`. They take the server's `ConnectedPlayer`, so they live in the server, next to the senders that call them, rather than with the session checks of `rtype_network`:

- `nextSessionHeader(player)` gives the header and moves the sequence on by one, wrapping from 65535 to 0.
- `sessionDatagram(player, message)` builds the datagram to the address of the player; an overload takes a message type and a payload already encoded, as the world state needs.
- `sessionHeaderFor(roster, endpoint)` gives the header of the player who sends from that address, or a header without session (token 0, sequence 0) when no player does.

The acceptance, the `EntitySpawned` of ships and Bydos, the waiting room, `GameStarted`, the world state and the game events all go through them, so the sequences a player receives grow by one from its acceptance, across every kind of message.

```cpp
send(rtype::server::sessionDatagram(player, rtype::network::GameStarted{}));
```

## A game: `GameInstance`

`GameInstance` (`src/server/Instances/GameInstance/`) groups everything that belongs to one game: its `World`, its `PlayerRoster` (players 1 to 4, whose numbers also pick their colors), its state and its tick. The server keeps a list of games; today `main.cpp` builds one, and every connection enters it. The tick handler calls `onTick(datagrams)` on each game with the datagrams of the admitted players, on the simulation thread, and `onPlayerJoined(player)` gives a new player its ship.

| State (`GameState`) | The game… | It goes to… |
|---|---|---|
| `WaitingForPlayers` | takes `ReadyState`, lets players in and sends them the room (see [Waiting room](#waiting-room-lobbystateannouncer)) | `InProgress` on the tick when at least one player is connected and every connected player is ready, which sends every connected player `GameStarted` |
| `InProgress` | counts one tick per `onTick`, runs the systems of the world, lets the Bydos appear and sends the world state; refuses new connections | `Finished` when `finish()` is called |
| `Finished` | refuses new connections | `WaitingForPlayers` after `FINISHED_STATE_TICKS` (300) ticks, `FINISHED_STATE_SECONDS` (5) seconds |

- Going back to `WaitingForPlayers` builds a new `World`: the tick is 0, the players stay connected, none is ready any more, and each gets a new ship, announced again. Network ids start again at 1 in the new world, so the announced ids are the ones of the previous game.
- `onTick` reads a datagram only when its header is of this protocol and version, its message sizes add up and its sender is a player of the game. It does not check the session token: the sorter already dropped, on the network thread, every datagram from the address of a player that does not carry its token (see [Sorting the datagrams](#sorting-the-datagrams-datagramsorter)), so a sender that forges the address of a player can neither say ready nor steer in its place. Every datagram it does not read is logged at the `debug` level under `Instances` with its sender and why, at most once per `REJECTION_LINE_INTERVAL` of the ticks the game ran: 600 ticks count as exactly 10 s.
- A `ReadyState` counts only while the game waits for players, and with a sequence newer than the newest one accepted from that player (`isNewerSequence`, 1 to 32767 ahead, wrapping from 65535 to 0; exactly 32768 ahead is never newer). A late copy therefore never undoes a newer one. Anything else is ignored.
- `finish()` ends the game in progress and does nothing in another state. Nothing calls it yet: no rule of the game decides that a game is over, so no game ends today.
- The tick that starts the game sends every connected player `GameStarted`, then the world state of tick 0, so `GameStarted` always leaves first and a lost one is corrected by that world state. The game logs `game started, players connected: <n>` at the `info` level under `Instances`.
- `GameEnded` is not sent yet.

```cpp
for (const auto& game : games) {
  game->onTick(received.players);
}
connectionAcceptor.setGameInProgress(
    joinableGame.state() != rtype::server::GameState::WaitingForPlayers);
connectionAcceptor.receive(received.connectionRequests, clock.now());
```

## Waiting room: `LobbyStateAnnouncer`

`LobbyStateAnnouncer` (`src/server/Replication/LobbyStateAnnouncer/`) tells the players of a game who is in the waiting room and who is ready, as a `LobbyState` (see [Protocol](/R-Type/protocol/#a-session)). `GameInstance` calls `onWaitingTick()` on every tick it waits for players, and builds a new announcer with each new world, so the room goes out at the first tick of every wait.

- The room lists every connected player, in order of number, with its ready state and its nickname, read from the `PlayerRoster` at each tick.
- It is sent to every connected player as soon as it differs from the room sent last: in the tick a `ReadyState` changes it, and in the tick after a player joins, since `main.cpp` answers the connection requests after the games ticked. When that `ReadyState` makes every player ready, the game starts in the same tick and sends `GameStarted` instead.
- An unchanged room is sent again `TICKS_PER_LOBBY_STATE` (30) ticks after the last send. `LOBBY_STATE_INTERVAL` (500 ms) is **provisional**, like the protocol rate it follows; `TICKS_PER_LOBBY_STATE` is that interval in whole ticks, rounded down, so 30 ticks are 499.99998 ms, while adding up tick durations until 500 ms would wait 31 ticks, 516.7 ms. Code must read the constants (`src/server/ServerConstants.hpp`) and never repeat 500 or 30.
- Nothing is sent while the game is in progress or finished, and a room without players reaches no one.

## Player ships: `ShipSpawnAnnouncer`

A player gets a ship the moment the `ConnectionAcceptor` lets them in. `ShipSpawnAnnouncer` (`src/server/Replication/ShipSpawnAnnouncer/`) creates it with `World::spawnPlayer(position)` (`rtype_game`), which gives the entity a `Position`, an `EntityType::Player` and a `NetworkIdentity` from the world's `NetworkIdAllocator`, and returns that network id. The id is kept in `ConnectedPlayer::shipEntityId` (`NO_ENTITY_ID` while the player has no ship).

`shipStartPosition(playerNumber, playerCount)` (`src/game/Spawning/`) says where the ship appears: at `SHIP_START_X_RATIO` (0.1) of the playfield width, so x is 192, and on a row of its own. The height is cut into `playerCount + 1` equal parts and player n takes the n-th cut, so with four players the rows are 216, 432, 648 and 864 logical units, each pair 216 apart. A number that is 0 or above `playerCount` throws `InvalidPlayerNumberException`.

The announcer then sends an `EntitySpawned` (`PlayerShip`, the position of the ship) in its own datagram, carrying the session token and the next sequence of the recipient:

- the new ship goes to every connected player, the new one included;
- the new player also gets one for each ship already in the game, so it sees the others. The position sent is the one the ship has in the world at that moment.
- A player who already has a ship gets no second one, and nothing is sent.
- A lost `EntitySpawned` is recovered from the world state (see [Protocol](/R-Type/protocol/#world-state)).
- Nothing removes a ship when a player leaves: the server does not handle leaving yet.

```cpp
rtype::server::ShipSpawnAnnouncer shipSpawnAnnouncer{world, roster,
                                                     queueDatagram};
rtype::server::ConnectionAcceptor connectionAcceptor{
    roster, queueDatagram,
    [&serverPointer](const rtype::network::Endpoint& player) {
      serverPointer->admit(player);
    },
    [&shipSpawnAnnouncer](rtype::server::ConnectedPlayer& player) {
      shipSpawnAnnouncer.onPlayerJoined(player);
    }};
```

## Player inputs

Each `GameInstance` keeps, in `ConnectedPlayer::newestInput`, the `PlayerInput` with the highest `inputNumber` each of its players sent, as the [Protocol](/R-Type/protocol/#inputs) requires. It reads them in `onTick`, through the same dispatcher and after the same checks as the `ReadyState` (see [A game](#a-game-gameinstance)), so each datagram is decoded once. The field is empty until the first input arrives. Nothing applies the input to the ship yet: the tick that does will read it through `PlayerRoster::connected()`.

- An input that is older, a duplicate, or numbered like the one kept changes nothing, so a datagram that arrives late or twice never undoes a newer input. Each player is compared with its own inputs only. Inputs are ordered by their `inputNumber`, not by the sequence of their datagram.
- Inputs are kept in every state of the game, and the input kept outlives a new world: the client numbers its inputs for the whole session, so its next input stays newer. Applying them only while the game is in progress is left to the code that moves the ships.
- The session token check of the sorter stops a sender that forges the address and port of a player: one forged input numbered 4294967295 would otherwise stay above every input the player sends for the rest of the session. It runs on the network thread, before the player queue, so a forged datagram takes no room in the queue either (see [Sorting the datagrams](#sorting-the-datagrams-datagramsorter)).
- `inputNumber` is compared as a plain number: at 60 inputs per second, it would take more than 800 days to wrap.
- A client that connects again from the same address and port gets back the same place and token, but numbers its inputs from 1 again: they would be ignored until they pass the input kept. No path of the client leads there today; when players can leave, leaving must forget the input kept.
- Each time the actions kept for a player change, the game logs `player <n> holds actions <byte>` at the `debug` level under `Instances`.

## Game events: `GameEventEncoder` and `GameEventBroadcaster`

The game announces what the players must see or hear by publishing an event on the `EventBus`; the server turns it into a message and sends it to every client. The server broadcasts five of the events in `src/game/Events/`: `EntitySpawn`, `EntityDestruction` (`Killed` or `Removed`), `EntityDamage`, `PlayerHit` and `PlayerElimination`. They carry a network id or a player number, never an `Entity`: whoever publishes reads the id from the `NetworkIdentity` before the entity is destroyed.

`GameEventEncoder` (`src/server/Replication/GameEventEncoder/`) writes an event as the message of the protocol (see [Protocol](/R-Type/protocol/#game-events)) and says how many times to send it. An `EntitySpawn` becomes an `EntitySpawned`, sent once, because the next world state creates the entity anyway. An `EntityDestruction` becomes an `EntityDestroyed`, and the other three become a `GameEvent` of kind `EntityDamaged`, `PlayerHit` or `PlayerEliminated`; these are sent `REPEATED_MESSAGE_COPIES` (3) times. The encoder numbers the `GameEvent`s from `FIRST_EVENT_NUMBER` (1), wrapping from 65535 to 0, and an event it refuses uses no number.

`GameEventBroadcaster` (`src/server/Replication/GameEventBroadcaster/`) subscribes to the five events and sends each message to every recipient, in a datagram whose header comes from the function it was given for that recipient. The first copy leaves when the bus delivers the event; the others leave `REPEATED_MESSAGE_INTERVAL` (50 ms) and twice that later, through the `TimerScheduler`.

- The copies go to the clients there were when the bus delivered the event: a client that joins after the delivery never receives a copy.
- The timers advance by one tick at a time, so a copy leaves on the first tick that reaches its delay, up to one tick (`SIMULATION_TICK_DURATION`, 16.7 ms) after it.
- `REPEATED_MESSAGE_COPIES` and `REPEATED_MESSAGE_INTERVAL` are **provisional**, like the protocol rates they follow. Code must read the constants and never repeat 3 or 50.
- An event the protocol refuses is skipped: a position that is not a number, the entity id 0 (which no entity has), or a player number outside 1 to 4. The broadcaster logs a warning with the name of the event, sends nothing, schedules no copy, and does not use an event number, so the next event goes out and the server keeps running. Only the encoding is protected: a fault in the sending still stops the tick.
- The broadcaster must outlive the last `dispatch()` of the bus and the last `advance()` of the timers, because both call back into it.
- Nothing is sent or scheduled when the list of recipients is empty, but the encoder still runs, so a `GameEvent` still uses an event number.
- In `main.cpp` the list holds the address of every player who joined the game, added when the acceptor lets the player in, and the header function is `GameInstance::nextSessionHeaderFor`, which reads the roster of that game through `sessionHeaderFor` (see [Session headers](#session-headers-sessiondatagram)), so each copy carries the token and the next sequence of its player. No player leaves the roster yet; when players can leave, leaving must also take them out of that list.
- `PlayfieldExitSystem` publishes an `EntityDestruction` when a Bydo leaves the playfield, so the players get its `EntityDestroyed`. `DamageRules` publishes two more events, but is not run.

```cpp
rtype::engine::EventBus events;
rtype::engine::TimerScheduler timers;
std::vector<rtype::network::Endpoint> recipients;
rtype::server::GameEventBroadcaster broadcaster{
    events, timers, recipients,
    [&game](const rtype::network::Endpoint& recipient) {
      return game.nextSessionHeaderFor(recipient);
    },
    send};

events.publish(rtype::game::PlayerHit{.playerNumber = 2});
timers.advance(rtype::engine::SIMULATION_TICK_DURATION);
events.dispatch();
```

## Bydos: `BydoSpawner`

`BydoSpawner` (`src/server/Spawning/BydoSpawner/`) makes a Bydo appear in the `World` every `BYDO_SPAWN_INTERVAL` (2 seconds, **provisional** until the waves set the pace) of simulation time. A `GameInstance` builds one with each world and ticks it only while the game is `InProgress`, so nothing appears while the game waits for players or is finished, and the timer starts again from zero with every new game. Time is counted in whole ticks of `SIMULATION_TICK_DURATION`, which is 16 666 666 ns, so the first Bydo of a game appears on the 121st tick, not the 120th.

- `bydoSpawnPosition(randomUnit)` (`src/game/Spawning/`) says where: `BYDO_SPAWN_MARGIN` (64 logical units) past the right edge of the playfield, so the Bydo is off screen, and at a height from `BYDO_SPAWN_VERTICAL_MARGIN` (64) to `PLAYFIELD_HEIGHT` minus that margin. A draw of 0 gives the highest row; a draw from 0 up to, but not including, 1 is accepted, anything else (1 included, or not a number) throws `InvalidRandomUnitException`. Both margins are **provisional**: they are half a Bydo until its size is fixed.
- `World::spawnBydo(position, kind)` gives the entity a `Position`, a `Velocity` of `-BYDO_SPEED` (240 units per second, **provisional**, to the left), a `MovementPattern`, `EntityType::Enemy`, `Health` and a `NetworkIdentity`. It has no `CollisionBox` yet. The spawner alternates the two `BydoKind`s, `Drifter` then `Weaver`, starting with the `Drifter`, and draws nothing for it, so a seed still gives the same heights. See [Bydo movement](#bydo-movement-patternsystem-and-playfieldexitsystem).
- Every connected player is sent an `EntitySpawned` (`Bydo`, the position), in its own datagram, like the ship of a player. Both use `sendEntitySpawned` (`src/server/Replication/EntitySpawnSender.hpp`). With no player connected, the Bydo appears and nothing is sent. When every network id was given, the spawn is skipped and a warning is logged.
- The generator is a `std::mt19937` started from a seed. The spawner logs it at the `info` level when it is built (`Bydo spawning starts from the seed 42`), so the same seed gives the same heights in the same order. `GameInstance` gives each world the seed of the game plus the number of worlds built before it, so one seed replays every game of a session; without `--seed`, the server draws it from `std::random_device`.

```bash
./r-type_server --seed 42
```

## Bydo movement: `PatternSystem` and `PlayfieldExitSystem`

Each tick of a game in progress, `GameInstance` runs three systems of the world, in this order, before the Bydos appear: `PatternSystem`, `MovementSystem`, then `PlayfieldExitSystem`. It runs them with `SIMULATION_TICK_DURATION`, and the scheduler is built once per game, so it survives the new worlds.

- A Bydo moves left at `BYDO_SPEED` through its `Velocity`, which `MovementSystem` applies. Its `MovementPattern` holds a `PatternShape`, the row it appeared on (`originY`) and its age in seconds.
- `PatternSystem` (`src/game/Systems/PatternSystem/`) sets the height of every entity that has a `MovementPattern`. `Straight` keeps the row. `Wave` puts the entity at `originY + WAVE_AMPLITUDE * sin(2 * pi * age / WAVE_PERIOD_SECONDS)`, with an amplitude of 48 units and a period of 2 seconds, both **provisional**. The height is computed from the age, never added up tick by tick, so it does not depend on how the time is cut into steps. The amplitude stays under `BYDO_SPAWN_VERTICAL_MARGIN`, checked at compile time, so a Bydo on a wave never leaves the height of the playfield.
- `patternShapeOf(kind)` (`src/game/Spawning/BydoKind/`) says which shape a kind follows: the `Drifter` goes `Straight`, the `Weaver` follows a `Wave`.
- `PlayfieldExitSystem` (`src/game/Systems/PlayfieldExitSystem/`) destroys an entity of type `Enemy` that has a `NetworkIdentity` once its `Position` is more than `BYDO_EXIT_MARGIN` (64 logical units, **provisional**) beyond the left edge, and publishes an `EntityDestruction` with the cause `Removed` under its network id. A Bydo exactly at the margin is still in. A ship is never removed by leaving. It takes the `EventBus` of the server, which `GameInstance` receives at construction and which must outlive it.
- A Bydo crosses the playfield in about 8.5 seconds, so about four or five Bydos are in transit at the pace of `BYDO_SPAWN_INTERVAL`.
- The players are sent an `EntityDestroyed` (`Removed`) for each Bydo removed, `REPEATED_MESSAGE_COPIES` (3) times (see [Game events](#game-events-gameeventencoder-and-gameeventbroadcaster)).

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

The events are structs in `src/game/Events/`. `MissileHit` and `ShipContact` say that a contact happened; what it costs is decided by [`DamageRules`](#damage-damagerules).

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

The bus and the registry must outlive `CollisionRules`, and the bus must outlive `CollisionSystem`. `main.cpp` creates a bus and delivers its events every tick, but nothing adds `CollisionSystem` or `CollisionRules` to it yet: today only the tests build this chain.

## Damage: `DamageRules`

`DamageRules` (`src/game/Systems/DamageRules/`) decides what a `MissileHit` costs. It subscribes to `MissileHit` when it is created and unsubscribes when it is destroyed. For each hit, in this order:

1. The missile is destroyed and announced with an `EntityDestruction` whose cause is `Removed`, so the clients show nothing.
2. If the target has a `Health`, it loses `MISSILE_DAMAGE` (1) points. While it has points left it is announced with an `EntityDamage`. At 0 or less it is destroyed and announced with an `EntityDestruction` whose cause is `Killed`, which the client shows as an explosion.
3. A target without `Health` is not hurt: the missile still disappears.

- A Bydo has `Health` of `BYDO_HEALTH` (3) when `World::spawnBydo` creates it, so three missiles destroy it. Both values are **provisional** until gameplay tunes them. Code must read the constants and never assume 1 or 3.
- The ship of a player has no `Health`: what a hit costs a player belongs to the lives of the players, which do not exist yet. A `ShipContact` is not handled either.
- Network ids are read from the `NetworkIdentity` before the entity is destroyed. A hit naming an entity that is already gone, or that has no `NetworkIdentity`, is ignored, so two missiles that reach the same Bydo in one `dispatch()` kill it once, and the second missile is left alone.
- The bus and the registry must outlive `DamageRules`. It publishes while the bus delivers, so its events reach `GameEventBroadcaster` in the same tick.
- Nothing builds it in `main.cpp` yet, and nothing spawns a missile: today only the tests run this chain.

```cpp
rtype::engine::EventBus events;
rtype::game::CollisionRules collisionRules{events, world.components()};
rtype::game::DamageRules damageRules{events, world.components()};
```

## Where to intervene

| You want to… | Look at |
|---|---|
| Change what a tick does | The tick handler given to `ServerApplication` in `src/server/main.cpp` |
| Change which datagrams reach the tick | `DatagramSorter::sort` in `src/server/Application/DatagramSorter/DatagramSorter.cpp` |
| Change how many connection requests a tick answers | `CONNECTION_REQUEST_QUEUE_CAPACITY` in `src/server/ServerConstants.hpp` |
| Add a launch option | The options listed in `LaunchOptions::read()` and `usage()`, in `src/server/LaunchOptions/LaunchOptions.cpp`; a name the client uses too goes in `src/engine/LaunchArguments/LaunchArgumentsConstants.hpp` |
| Change the port the server listens on by default | `DEFAULT_SERVER_PORT` in `src/network/NetworkConstants.hpp`, shared with the client |
| Change how often the world state is sent | `WORLD_STATES_PER_SECOND` in `src/server/ServerConstants.hpp` |
| Change how often an unchanged waiting room is sent again | `LOBBY_STATE_INTERVAL` in `src/server/ServerConstants.hpp` |
| Change what the waiting room lists | `roomOf` in `src/server/Replication/LobbyStateAnnouncer/LobbyStateAnnouncer.cpp` |
| Send a datagram to a player during a session | `sessionDatagram` in `src/server/Replication/SessionDatagram.hpp`, then the send function of the game |
| Announce something to every client | Publish one of the events of `src/game/Events/` on the `EventBus`; `GameEventBroadcaster` sends it |
| Send a new kind of event | A struct in `src/game/Events/`, an `encode` overload in `GameEventEncoder`, a `subscribeTo` line in the `GameEventBroadcaster` constructor |
| Change how often an event is sent, or how far apart | `REPEATED_MESSAGE_COPIES` and `REPEATED_MESSAGE_INTERVAL` in `src/server/Replication/GameEventConstants.hpp` |
| Change the health of a Bydo, or the damage of a missile | `BYDO_HEALTH` in `src/game/Components/HealthConstants.hpp`, `MISSILE_DAMAGE` in `src/game/Systems/DamageRules/DamageConstants.hpp` |
| Change what a hit does | `DamageRules::onMissileHit` in `src/game/Systems/DamageRules/DamageRules.cpp` |
| Change what the world state lists | `WorldStateSender::capture` in `src/server/Replication/WorldStateSender/WorldStateSender.cpp` |
| Send a datagram from the tick | `ServerApplication::queueDatagram()` |
| Change how a connection request is answered | `ConnectionAcceptor::answer` in `src/server/Lobby/ConnectionAcceptor/ConnectionAcceptor.cpp` |
| Change where a ship appears | `shipStartPosition` and `SHIP_START_X_RATIO` in `src/game/Spawning/` |
| Change how often a Bydo appears | `BYDO_SPAWN_INTERVAL` in `src/server/Spawning/BydoSpawnerConstants.hpp` |
| Change where a Bydo appears | `bydoSpawnPosition` and the margins in `src/game/Spawning/` |
| Change how fast a Bydo moves, or the height and period of a wave | `BYDO_SPEED`, `WAVE_AMPLITUDE`, `WAVE_PERIOD_SECONDS` in `src/game/Systems/PatternSystem/PatternConstants.hpp` |
| Change which pattern a kind of Bydo follows | `patternShapeOf` in `src/game/Spawning/BydoKind/BydoKind.cpp` |
| Change when a Bydo leaves the playfield | `BYDO_EXIT_MARGIN` in `src/game/Spawning/SpawningConstants.hpp`, `PlayfieldExitSystem` |
| Replay a game | Start the server with the seed found in its log: `--seed <seed>` |
| Change who is told about a new ship | `ShipSpawnAnnouncer` in `src/server/Replication/ShipSpawnAnnouncer/` |
| Change when a game starts or ends, what the players are told at the start, or what is reset between two games | `GameInstance::onTick`, `startGame` and `startNewWorld` in `src/server/Instances/GameInstance/GameInstance.cpp` |
| Change how long a game stays finished | `FINISHED_STATE_SECONDS` in `src/server/Instances/GameInstanceConstants.hpp` |
| Change how players get their number, or what the server keeps of them | `PlayerRoster` in `src/server/Instances/PlayerRoster/`, `ConnectedPlayer` in `src/server/Instances/ConnectedPlayer.hpp` |
| Read the input a player holds | `ConnectedPlayer::newestInput`, kept by its `GameInstance` |
| Change which input of a player the server keeps | `isNewer` in `src/server/Instances/GameInstance/GameInstance.cpp` |
| Change which datagrams of the players a game reads | `GameInstance::readPlayerDatagram` in `src/server/Instances/GameInstance/GameInstance.cpp` |
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
| Change how entities move | `MovementSystem` in `src/game/Systems/MovementSystem/` |
| Add a component type | A new struct in `src/game/Components/` |
| Write a test with entities that collide | `tests/CollisionScene.hpp` |

Each folder holding sources has its own `CMakeLists.txt` declaring one library; `Application/` and `Platform/` only add their subfolders, `Replication/` declares its constants and the session header (`rtype_server_session_datagram`), and adds one folder per sender (`WorldStateSender/`, `LobbyStateAnnouncer/`, `ShipSpawnAnnouncer/`, the event encoder and broadcaster…), `Lobby/` declares its constants and adds `ConnectionAcceptor/`, `Instances/` declares `ConnectedPlayer`, `GameState` and the game constants and adds `PlayerRoster/` and `GameInstance/`, and `r-type_server` links `rtype_server_application`, `rtype_server_connection_acceptor`, `rtype_server_game_event_broadcaster`, `rtype_server_game_instance` and `rtype_server_launch_options` in `src/server/CMakeLists.txt`.
