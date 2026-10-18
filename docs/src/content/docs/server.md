---
title: Server and game
description: What the server program and the game library do, how their parts fit together and where to find them in the code.
---

The server, `r-type_server`, is meant to run the game for the players connected to it. Today it receives their datagrams, runs a tick at a fixed rate and lets up to four players in: each tick keeps the newest input of each player and answers the connection requests received. Nothing else a client sends is read yet, nothing applies the inputs or advances the `World`, and no one is on the recipient list of the world state, so no state is sent yet. It never links SFML or Asio: it reaches the network through `rtype_network`, and its own libraries are checked for it at configure time (see [Project layout](/R-Type/project-layout/#who-may-link-what)).

The game library, `rtype_game`, holds the rules of a match: what moves, what touches what and what that means. It is built on the engine (see [Engine](/R-Type/engine/)), and it knows nothing about sockets, SFML or the messages sent between the programs (see [Project layout](/R-Type/project-layout/#game)). The server runs all of it; the client will only run the part that predicts its own ship.

This page tells you where to look when you need to change something. It describes what exists today and says plainly what does not exist yet.

| Part | State | Where |
|---|---|---|
| Network thread and simulation thread | Available | `src/server/Application/` |
| Timer resolution on Windows | Available | `src/server/Platform/` |
| Sorting the datagrams before the queues | Available; no limit per address yet | `src/server/Application/DatagramSorter/` |
| World state sent at a fixed rate | Available: sends to no one yet, the list of recipients is empty | `src/server/Replication/` |
| Launch options | Available: the port and `--help` | `src/server/LaunchOptions/` |
| Letting players in: challenge, acceptance or refusal | Available; nothing tells it a game is in progress yet | `src/server/Lobby/`, `src/server/Instances/` |
| Newest input of each player, session token checked | Available; nothing applies it to the ship yet | `src/server/Instances/PlayerInputReceiver/` |
| Lobby state, games, master link | Not yet written | — |
| Components (`Position`, `Velocity`, `CollisionBox`, `EntityType`, `Sprite`, `Layer`) | Available | `src/game/Components/` |
| Movement | Available | `src/game/Systems/MovementSystem/` |
| Collisions | Available: nothing runs them yet | `src/game/Systems/CollisionSystem/`, `src/game/Systems/CollisionRules/`, `src/game/Events/` |
| Damage and destruction | Not yet written | — |
| Ship of each player, created when the player joins and announced to the others | Available | `src/game/Spawning/`, `src/server/Replication/ShipSpawnAnnouncer/` |
| Enemies, waves, score, lives | Not yet written | — |

## Launch options: `LaunchOptions`

`LaunchOptions` (`src/server/LaunchOptions/`) reads what the server is told at launch. `main()` first reads the logging options (see [Logging](/R-Type/logging/#choosing-the-level-and-the-output-at-launch)), then hands the other arguments to `LaunchOptions::read()`, which reads them with the engine's `readLaunchArguments` (see [Engine](/R-Type/engine/#launch-arguments-readlauncharguments)):

| Option | Effect |
|---|---|
| `-p <port>` | The UDP port to listen on, from 1 to 65535 (see [Network](/R-Type/network/#ports-and-addresses-written-as-text-portfromtext-ipv4addressfromtext)). Default: `DEFAULT_SERVER_PORT` (4242). |
| `--help` | Prints the usage, the logging options included, on standard output, then exits with code 0 without starting anything or opening the journal. |

- The server listens on every IPv4 interface (`ANY_IPV4_ADDRESS`) and takes no address: `-h`, the client's address option, is an unknown argument here. The last `-p` wins.
- An unknown argument, a `-p` without a value or an invalid port stops the server before anything starts: it prints `r-type_server:`, the reason and how to see the usage on standard error, then exits with code 1.

```bash
./r-type_server -p 4243
```

## Threads: `ServerApplication`

`ServerApplication` (`src/server/Application/ServerApplication/`) runs the server on two threads, which never share game state:

- The **network thread**, the one that calls `run()`, runs the `NetworkContext`. A `DatagramSorter` sorts every datagram as it arrives: the datagrams of admitted players go to the player queue, connection requests to the request queue, and the rest is dropped there (see [Sorting the datagrams](#sorting-the-datagrams-datagramsorter)). `DatagramRelay` (`rtype_network`, see [Network](/R-Type/network/#socket-to-queues-datagramrelay)) sends what was left in the outgoing queue. It is the only thread that touches the socket.
- The **simulation thread** runs `SIMULATION_TICKS_PER_SECOND` (60) ticks per second. Each tick drains the player queue and the request queue and hands what arrived, often nothing, to the tick handler given to the constructor, as a `ReceivedDatagrams` (`players`, `connectionRequests`), so a slow or silent player never holds the game up. At the end of the tick it asks the network thread, through `NetworkContext::post()`, to send the outgoing queue.

The tick handler is where the game plugs in. Today `src/server/main.cpp` gives one that hands the datagrams of the players to the `PlayerInputReceiver`, which keeps the newest input of each (see [Player inputs](#player-inputs-playerinputreceiver)), hands the connection requests to the `ConnectionAcceptor`, which answers them and gives each new player a ship (see [Letting players in](#letting-players-in-connectionacceptor-playerroster) and [Player ships](#player-ships-shipspawnannouncer)), then calls the `WorldStateSender`, which has no recipient yet.

A tick handler puts a datagram in the outgoing queue with `queueDatagram()`, called from the handler itself, on the simulation thread. The datagram is sent at the end of the same tick.

The sorter only lets through datagrams of this protocol whose message sizes add up, but anyone can still send a connection request, and the payloads are read by the tick, so the tick handler must drop and count a datagram it cannot decode, and never let an exception caused by a datagram's content escape: an exception stops the server (see below), so one truncated datagram would stop it for every player. `ByteReader`, for instance, throws `BufferUnderflowException` when a datagram is shorter than what it reads.

The player queue and the outgoing queue hold `INCOMING_DATAGRAM_QUEUE_CAPACITY` and `OUTGOING_DATAGRAM_QUEUE_CAPACITY` (256) datagrams each, about one second of the inputs of four players; the request queue holds `CONNECTION_REQUEST_QUEUE_CAPACITY` (16) (`src/server/ServerConstants.hpp`). These sizes are **provisional**: they will be revised with the rates of the protocol. Code must read the constants.

- When more datagrams arrive between two ticks than the player queue or the request queue holds, the oldest are discarded. Every `TICKS_PER_REPORT` ticks (600, so every ten seconds), the server logs one warning per queue with how many were discarded, if any, and one with how many datagrams the sorter dropped before any queue (see below). The sender of a dropped datagram is not logged yet.
- A flood of connection requests, or of datagrams from strangers, never pushes a player's datagram out of its queue. It still delays the requests of new players: nothing limits the requests of one address yet (see [Flood protection](#flood-protection)).
- The outgoing queue also discards its oldest datagram when full, but nothing reports it yet.

`stop()` makes `run()` return, from any thread: the network thread stops at once, the simulation thread when its current sleep ends, about one tick later. An exception thrown on either thread, by the tick handler for instance, stops both, and `run()` rethrows it.

### Sorting the datagrams: `DatagramSorter`

`DatagramSorter` (`src/server/Application/DatagramSorter/`) decides, on the network thread, which queue each received datagram goes to, before any queue holds it. It reads the header and the type of each message, never a payload.

| The datagram… | Goes to |
|---|---|
| is not of this protocol and version, has a flag set, or its message sizes do not add up | nowhere: dropped |
| holds a `ConnectionRequest`, whoever sent it | the request queue, where the `ConnectionAcceptor` answers it (see [Letting players in](#letting-players-in-connectionacceptor-playerroster)) |
| comes from an admitted player | the player queue |
| anything else | nowhere: dropped |

- A player is admitted by `ServerApplication::admit(endpoint)`, which the acceptor calls once for each sender it gives a place to. It may be called from any thread: it posts the admission to the network thread, so a datagram the player sends before that is dropped like a stranger's. No player is ever forgotten yet: nobody leaves the game.
- The `PlayerRoster` and the `ConnectionAcceptor` run in the tick, on the simulation thread; the network thread only keeps the address and port of each admitted player.
- Each datagram the sorter drops is counted (`droppedCount()`, readable from any thread), and the server reports the count every ten seconds with the discards of its queues.
- Measured on an Apple Silicon laptop (Apple M4 Pro, `-O2`): sorting a datagram costs about 20 ns when it is dropped, 2 to 20 ns when it is malformed, and 50 to 60 ns when it is queued, copy included, so a flood of 3 800 datagrams per tick costs the network thread about 0.2 ms of every 16.7 ms, whatever the datagrams hold. A malformed datagram is found without throwing (`DatagramCodec::messageTypes`): an exception costs about 4 µs, so a flood of malformed datagrams would otherwise take about 14 ms of every tick. Answering a request costs about 0.45 µs, or 0.55 µs with a wrong cookie; the request queue keeps that under 9 µs per tick.

### Flood protection

The room of the player queue protects the server's memory; the sorter protects the players from the senders that were not admitted: one of them flooding the server's port with connection requests or with anything else no longer pushes the players' datagrams out. A sender that uses the address and port of an admitted player, that player itself or someone forging them, still can: the session token is only checked in the tick, after the queue (see [Player inputs](#player-inputs-playerinputreceiver)), and the players share one queue (layer 2 below). A plain Python loop on the loopback address sends about 3 800 datagrams per tick (measured on an Apple Silicon laptop), about fifteen times the room of the queue. Other engines protect the players in three layers; the server must have them once sessions exist, and the first and the third are written, in part:

1. **Sort on the network thread, before the queue.** Written: `DatagramSorter` drops there every datagram that is neither from an admitted player nor a `ConnectionRequest`, and the requests have their own queue. Not written: a limit on the requests of each address, in a table of fixed size so that forged addresses cannot make it grow. Quake 3 ignores such datagrams and answers at most 10 status, information or remote console requests per second from one address; Unreal limits how many datagrams of senders without a connection it processes each frame.
2. **One queue per session**, so a flooding client only fills its own. Valve's GameNetworkingSockets keeps one per connection, of 1 000 messages by default.
3. **One input per player and per tick.** The protocol requires it (see [Protocol](/R-Type/protocol/#inputs)). Written: `PlayerInputReceiver` keeps only the newest `PlayerInput` of each player and ignores older ones (see [Player inputs](#player-inputs-playerinputreceiver)). Not written: applying it at every tick.

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

`WorldStateSender` (`src/server/Replication/WorldStateSender/`) sends the state of the game to the players, at a fixed rate. The tick handler calls `onTick(components, recipients)` once per tick, on the simulation thread. The sender counts the ticks, and every `TICKS_PER_WORLD_STATE` (3) of them, that is `WORLD_STATES_PER_SECOND` (20) times per second, it lists every entity that has a `NetworkIdentity`, a `Position` and an `EntityType`, has `WorldStateCodec` encode the list (see [Network](/R-Type/network/#world-state-worldstatecodec)), and hands one `OutgoingDatagram` per recipient and per payload to the function it was given. In `main.cpp` that function is `queueDatagram()`.

- The `tick` of a state is the number of ticks the sender has counted, so the first state carries tick 3 and the client can tell which of two states is newer. The counter is 32 bits and is not made to wrap: it takes 2.27 years at 60 ticks per second.
- `WORLD_STATES_PER_SECOND` is **provisional**: it will be revised once measured on a real network. It must divide the tick rate, and the build fails otherwise. Code must read the constants, `TICKS_PER_WORLD_STATE` for instance, and never repeat the 3.
- Nothing is built when the list of recipients is empty. An entity without a network id, or without a type, is not listed.
- An entity whose position is not a finite number (NaN or infinite), which only a game bug can produce, is left out of the state instead of stopping the server. A warning under the `Replication` module gives how many entities were left out, each time that number changes, so a stuck entity is reported once rather than 20 times per second.
- The state lists no player yet: the game has no lives, score or player numbers. The ship of each player has a `NetworkIdentity` (see [Player ships](#player-ships-shipspawnannouncer)), so it is listed, but the list of recipients is still empty in `main.cpp`, so no state is sent.
- The datagrams hold the bare message payload, without the packet header, so no client could accept them yet.
- A playfield larger than 2047 logical units in either direction fails the build, because a wire coordinate would no longer hold it (see [Protocol](/R-Type/protocol/#positions)).

```cpp
rtype::server::ServerApplication* serverPointer = nullptr;
rtype::server::WorldStateSender worldStateSender{
    [&serverPointer](rtype::network::OutgoingDatagram datagram) {
      serverPointer->queueDatagram(std::move(datagram));
    }};
rtype::server::ServerApplication server{
    endpoint,
    [&worldStateSender, &world, &recipients](
        const rtype::server::ServerApplication::ReceivedDatagrams&) {
      worldStateSender.onTick(world.components(), recipients);
    }};
serverPointer = &server;
```

The sender is built before the server, whose tick handler captures it, and sends through the server, so a pointer set once the server exists closes the loop; it is set before `run()` starts the simulation thread.

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
- An acceptance carries the session token and the next sequence of that player, 0 for the first. Challenges and refusals carry token 0 and sequence 0, as every datagram sent before a session.
- Besides the function that admits a sender, the acceptor takes an optional one it calls once for each player it lets in, after it sent the acceptance, and not again when an acceptance is repeated. `main.cpp` gives it the `ShipSpawnAnnouncer`.
- Nothing caused by the content of a datagram makes it throw. Each new player is logged at the `info` level under `Lobby`, as `player <n> joined from <address> port <port>`; challenges and refusals at the `debug` level.
- Each new player is announced once to the function given to the acceptor, which `main.cpp` hands to `ServerApplication::admit()`, so the datagrams of that player reach the tick from then on.
- `setGameInProgress(bool)` turns the `GameInProgress` refusal on and off; nothing calls it yet, since games have no lifecycle. No player ever leaves the roster yet either: heartbeats and departures do not exist.

## Player ships: `ShipSpawnAnnouncer`

A player gets a ship the moment the `ConnectionAcceptor` lets them in. `ShipSpawnAnnouncer` (`src/server/Replication/ShipSpawnAnnouncer/`) creates it with `World::spawnPlayer(position)` (`rtype_game`), which gives the entity a `Position`, an `EntityType::Player` and a `NetworkIdentity` from the world's `NetworkIdAllocator`, and returns that network id. The id is kept in `ConnectedPlayer::shipEntityId` (`NO_ENTITY_ID` while the player has no ship).

`shipStartPosition(playerNumber, playerCount)` (`src/game/Spawning/`) says where the ship appears: at `SHIP_START_X_RATIO` (0.1) of the playfield width, so x is 192, and on a row of its own. The height is cut into `playerCount + 1` equal parts and player n takes the n-th cut, so with four players the rows are 216, 432, 648 and 864 logical units, each pair 216 apart. A number that is 0 or above `playerCount` throws `InvalidPlayerNumberException`.

The announcer then sends an `EntitySpawned` (`PlayerShip`, the position of the ship) in its own datagram, carrying the session token and the next sequence of the recipient:

- the new ship goes to every connected player, the new one included;
- the new player also gets one for each ship already in the game, so it sees the others. The position sent is the one the ship has in the world at that moment.
- A player who already has a ship gets no second one, and nothing is sent.
- A lost `EntitySpawned` is recovered from the world state (see [Protocol](/R-Type/protocol/#world-state)), which is not sent to anyone yet.
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

## Player inputs: `PlayerInputReceiver`

`PlayerInputReceiver` (`src/server/Instances/PlayerInputReceiver/`) reads the datagrams of the players each tick and keeps, in `ConnectedPlayer::newestInput`, the `PlayerInput` with the highest `inputNumber` each player sent, as the [Protocol](/R-Type/protocol/#inputs) requires. The field is empty until the first input arrives. Nothing applies the input to the ship yet: the tick that does will read it through `PlayerRoster::connected()`.

| The datagram… | Effect |
|---|---|
| is not of this protocol and version, has a flag set, or its message sizes do not add up | ignored |
| comes from a sender that is not in the roster | ignored |
| carries another session token than the one of its player | ignored and counted (`foreignTokenCount()`) |
| anything else | each `PlayerInput` it holds replaces the input kept if its number is higher; any other message is skipped |

- An input that is older, a duplicate, or numbered like the one kept changes nothing, so a datagram that arrives late or twice never undoes a newer input. Each player is compared with its own inputs only.
- The token check stops a sender that forges the address and port of a player: one forged input numbered 4294967295 would otherwise stay above every input the player sends for the rest of the session. It runs in the tick, after the player queue, so a forged datagram still takes room in the queue (see [Flood protection](#flood-protection)). The sequence is not checked yet.
- `inputNumber` is compared as a plain number: at 60 inputs per second, it would take more than 800 days to wrap.
- A client that connects again from the same address and port gets back the same place and token, but numbers its inputs from 1 again: they would be ignored until they pass the input kept. No path of the client leads there today; when players can leave, leaving must forget the input kept.
- Each time the actions kept for a player change, it logs `player <n> holds actions <byte>` at the `debug` level under `Instances`; a datagram with another token is logged at the `debug` level too.

```cpp
rtype::server::PlayerRoster roster;
rtype::server::PlayerInputReceiver playerInputReceiver{roster};
const auto handleTick =
    [&playerInputReceiver](
        const rtype::server::ServerApplication::ReceivedDatagrams& received) {
      playerInputReceiver.receive(received.players);
    };
```

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

The events are structs in `src/game/Events/`. `MissileHit` and `ShipContact` say that a contact happened; what it costs is left to the damage rules, which do not exist yet.

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

The bus and the registry must outlive `CollisionRules`, and the bus must outlive `CollisionSystem`. The game loop that would create them does not exist yet: today only the tests build this chain.

## Where to intervene

| You want to… | Look at |
|---|---|
| Change what a tick does | The tick handler given to `ServerApplication` in `src/server/main.cpp` |
| Change which datagrams reach the tick | `DatagramSorter::sort` in `src/server/Application/DatagramSorter/DatagramSorter.cpp` |
| Change how many connection requests a tick answers | `CONNECTION_REQUEST_QUEUE_CAPACITY` in `src/server/ServerConstants.hpp` |
| Add a launch option | The options listed in `LaunchOptions::read()` and `usage()`, in `src/server/LaunchOptions/LaunchOptions.cpp`; a name the client uses too goes in `src/engine/LaunchArguments/LaunchArgumentsConstants.hpp` |
| Change the port the server listens on by default | `DEFAULT_SERVER_PORT` in `src/network/NetworkConstants.hpp`, shared with the client |
| Change how often the world state is sent | `WORLD_STATES_PER_SECOND` in `src/server/ServerConstants.hpp` |
| Change what the world state lists | `WorldStateSender::capture` in `src/server/Replication/WorldStateSender/WorldStateSender.cpp` |
| Send a datagram from the tick | `ServerApplication::queueDatagram()` |
| Change how a connection request is answered | `ConnectionAcceptor::answer` in `src/server/Lobby/ConnectionAcceptor/ConnectionAcceptor.cpp` |
| Change where a ship appears | `shipStartPosition` and `SHIP_START_X_RATIO` in `src/game/Spawning/` |
| Change who is told about a new ship | `ShipSpawnAnnouncer` in `src/server/Replication/ShipSpawnAnnouncer/` |
| Change how players get their number, or what the server keeps of them | `PlayerRoster` in `src/server/Instances/PlayerRoster/`, `ConnectedPlayer` in `src/server/Instances/ConnectedPlayer.hpp` |
| Read the input a player holds | `ConnectedPlayer::newestInput`, kept by `PlayerInputReceiver` |
| Change which input of a player the server keeps | `isNewer` in `src/server/Instances/PlayerInputReceiver/PlayerInputReceiver.cpp` |
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

Each folder holding sources has its own `CMakeLists.txt` declaring one library; `Application/` and `Platform/` only add their subfolders, `Replication/` declares its constants and adds `WorldStateSender/` and `ShipSpawnAnnouncer/`, `Lobby/` declares its constants and adds `ConnectionAcceptor/`, `Instances/` declares `ConnectedPlayer` and its constants and adds `PlayerRoster/` and `PlayerInputReceiver/`, and `r-type_server` links `rtype_server_application`, `rtype_server_connection_acceptor`, `rtype_server_launch_options`, `rtype_server_player_input_receiver`, `rtype_server_ship_spawn_announcer` and `rtype_server_world_state_sender` in `src/server/CMakeLists.txt`.
