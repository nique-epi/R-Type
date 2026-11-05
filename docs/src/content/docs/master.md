---
title: Master
description: What the master does today, how to start it, and where to add a route, a table or a module.
---

The master is the program that lists the game servers and will direct them: a host creates its server on the master and gets its key, then the server registers with that key and tells the master, every few seconds, which games it hosts and who plays them; a player will read that list in the client, and an admin will send orders from the web site. It is a mode of the server binary, `r-type_server --mode master`, and it never runs a game: no datagram of a game goes through it, so the players of a server keep playing when the master stops. Why it speaks HTTP and why it is a mode rather than a binary of its own is told in [Architecture](/R-Type/architecture/#master).

This page tells you where to look when you need to change something. It describes what exists today and says plainly what does not exist yet.

| Part | State | Where |
|---|---|---|
| Start and stop: `--mode master`, `master.cfg`, Ctrl-C and SIGTERM | Available | `src/server/main.cpp`, `src/master/LaunchOptions/` |
| HTTP API under `/api`, every error in JSON | Available | `src/master/Http/` |
| Database `r-type_master.db` | Available: servers, samples, changes of status | `src/master/Storage/` |
| Creation of a game server and its key, `POST /api/servers` | Available; the web site that calls it is not written | `src/master/Servers/` |
| Registration and heartbeats of the game servers | Available; sent by the module `master` of the game server | `src/master/Servers/`, `src/network/Master/`, `src/server/Master/` |
| The client of the master, `MasterClient` | Available | `src/network/Master/Client/` |
| The games of a server, for the client, `GET /api/servers/{id}/games` | Available | `src/master/Servers/`, `src/network/Master/` |
| The statuses of the servers, their list and their history, `GET /api/servers` | Available; the status `unreachable`, which needs a probe over UDP, and the route that puts a server in maintenance are not written | `src/master/Servers/`, `src/network/Master/` |
| Administration, global leaderboard, accounts, web site | Not yet written | |

## Starting the master

`--mode master` comes first, before every other option; `r-type_server` alone, or `--mode server`, runs the game server as before (see [Server](/R-Type/server/#launch-options-launchoptions)). `chooseServerMode()` (`src/server/LaunchOptions/`) reads the mode, then `MasterLaunchOptions` (`src/master/LaunchOptions/`) reads the rest in one pass, the logging options included (see [Logging](/R-Type/logging/#choosing-the-level-and-the-output-at-launch)).

```bash
./r-type_server --mode master -p 8081
```

| Option | Effect |
|---|---|
| `-p <port>` | The TCP port of the HTTP API, from 1 to 65535, as the setting `port`. Default: `DEFAULT_MASTER_PORT` (8080). |
| `--database <file>` | The SQLite file of the master, on a local disk, as the setting `database`. Default: empty, which keeps it in `r-type_master.db` next to the executable. |
| `--set <name>=<value>` | Gives a setting a value, as the line `set <name> <value>` of `master.cfg` does. Can be given several times. |
| `--help` | Prints the usage of the master, the logging options included, then exits with code 0. |

- The journal is `r-type_master.log` next to the executable, unless a logging option says otherwise; in a folder the master cannot write to, it writes no journal and says so in the terminal, as the server does (see [Logging](/R-Type/logging/)).
- An unknown mode, a `--mode` without a value, an unknown argument or an invalid port stops the binary before anything starts, with code 1. A `--mode` that is not the first argument is an unknown argument of the server.
- Ctrl-C or SIGTERM stops the master with code 0, as the server stops (see [Server](/R-Type/server/#stopping-ctrl-c-and-sigterm)). A module that fails to start, such as a port another program holds, stops it with code 1 and a message naming the module.

## Configuration: `master.cfg`

The master reads `master.cfg`, in the folder it is started from, between the install and the start of its modules, in the language of `server.cfg` (see [Configuration commands](/R-Type/commands/)). A missing file is not an error. `master.cfg.example`, at the root of the repository, lists both settings with their default.

| Setting | Value | Default | Effect |
|---|---|---|---|
| `port` | 1 to 65535 | 8080 | The TCP port the HTTP API listens on, on every interface. |
| `database` | a path | empty | The SQLite file of the master; empty for `r-type_master.db` next to the executable. |

## Modules

`masterModules()` (`src/master/Modules/`) lists the modules of the master, in the order they install and start; they stop in the reverse order. The master runs no stage of a loop (`MASTER_LOOP_STAGES` is empty): the threads of the HTTP server answer every request.

| Module | Class | Needs | Opens when it starts |
|---|---|---|---|
| `logger` | `MasterLoggerModule` | nothing | the journal |
| `storage` | `MasterStorageModule` | `logger` | the database, migrated |
| `http` | `MasterHttpModule` | `logger` | the port, and the thread that listens on it |
| `servers` | `ServersModule` | `logger`, `storage`, `http` | `StatusWatch`, the thread of `ServerStatusJob`; its routes run once `http` listens |

The storage starts before the HTTP API, so a database the master cannot open stops it before its port opens, and the API stops before the database closes.

## The HTTP API: `MasterHttpServer`

`MasterHttpServer` (`src/master/Http/`) wraps cpp-httplib, which no other file of the master includes. Its routes sit under `/api`, answer `GET`, `POST` or `PUT`, and answer `application/json`; each is declared once, with its method and path, in `MasterRouteConstants.hpp` (`src/network/Master/Routes/`), which the master and `MasterClient` both read; they run on the threads of cpp-httplib, at least 8 of them, and must not block for long. A route reads an `ApiRequest`: the body, the key of an `Authorization: Bearer <key>` header (RFC 6750, the scheme in any case) and the address of the TCP peer. It answers an `ApiResponse`: a status, a body, the `detail` of a refusal and one header, such as `Location`.

`GET /api/health` answers 200 with the version of the master, `MASTER_VERSION`:

```json
{"version":"0.1.0"}
```

- **Errors:** an answer of 400 or more without a body gets a problem document of RFC 9457, `application/problem+json`, whose `title` is the phrase of its status, with the `detail` of the route when it gives one: `{"status":404,"title":"Not Found"}` for a path no route answers.
- **The address of the peer** is the one of the TCP connection, never one an `X-Forwarded-For` header names: no proxy is trusted.
- **A route that throws** answers 500, and its reason goes to the journal only, never to the client.
- **A body over 64 KiB** (`MAX_REQUEST_BODY_BYTES`) sent to a route answers 413 without being kept whole; cpp-httplib would keep up to 100 MiB. A path no route answers gets 404 without its body being read.
- **One master per port:** by default cpp-httplib sets `SO_REUSEPORT`, which lets a second program bind the port of the first with no error. The master sets `SO_REUSEADDR` instead (`SO_EXCLUSIVEADDRUSE` on Windows), so a second master on the same port stops at its start, with an error naming the port.
- **No encryption:** the master listens in plain HTTP on every IPv4 interface (`ANY_IPV4_ADDRESS`), for a local network. On the Internet, a reverse proxy such as Caddy or nginx in front of it must add HTTPS.

## The database: `MasterStorageModule`

The storage module opens its file with `rtype_storage` (`openDatabaseFile()`, see [Project layout](/R-Type/project-layout/#storage)): write-ahead log, foreign keys on, and the migrations of `MASTER_MIGRATIONS` (`src/master/Storage/MasterStorageConstants.hpp`) the file does not hold yet. A file it cannot open, or one written by a newer master, stops the start with an error naming the file. It provides the connection as a `MasterDatabase`, which the threads of the HTTP server share behind one lock: a request holds it for a few statements of one row each.

| Version | Table | Holds |
|---|---|---|
| 1 | `servers` | each created server: its name, the fingerprint of its key, the address and the date of its creation; once it registered, what it registered and the last heartbeat it sent, nicknames included |
| 1 | `heartbeat_samples` | the measures of each heartbeat, at most one per server and per second, kept 7 days |
| 2 | `servers` | the status computed last, whether an admin put the server in maintenance, and the games, players, tick p95 and loss of the last heartbeat, written with it so the list and the statuses parse no JSON |
| 2 | `status_changes` | each change of status of each server, at most one per server and per second, kept 7 days |

## Creating a game server: `POST /api/servers`

A host creates its server on the web site, which calls `POST /api/servers` with the name players will read; the web site is not written yet, so today the call is made by hand or by `MasterClient::createServer()`. The master answers 201, `Location: /api/servers/<id>`, and the server with its key:

```json
{"id":7,"name":"Paris 1","key":"rtk_<64 hexadecimal digits>"}
```

- **The key is shown this once.** It is `rtk_`, so a key that leaks in a log or a repository is recognized, then `SERVER_KEY_BYTES` (32) drawn by `hydro_random_buf()` of libhydrogen, which seeds itself from the system (`/dev/urandom` on Linux once `/dev/random` is ready, `/dev/urandom` on macOS, `RtlGenRandom` on Windows), in lowercase hexadecimal: 68 characters. It never goes in a URL or in the journal.
- **The database keeps its fingerprint only**, `hydro_hash_hash()` of the key in the context `FINGERPRINT_CONTEXT`, so a copy of the file lets nobody act as a server. A key is 256 random bits, so a fast hash is enough: nobody can try every key, unlike every password.
- **One address creates `MAXIMUM_CREATIONS_PER_ADDRESS` (5) servers per `CREATION_WINDOW` (1 hour) at most**; the next one answers 429. The count and the insert are one transaction, so requests sent at once cannot pass the quota together. The address is the one of the TCP peer: behind a reverse proxy, every host shares the quota of the proxy.
- **Anyone who reaches the master may create a server** until accounts exist: the web site will then ask for a signed-in host. Revoking a key, and giving it an expiry, are not written yet.

The host then writes the key in the `server.cfg` of that server, as the setting `master-key`, next to `master-url`; `server.cfg` is never committed, only `server.cfg.example`, with an empty key. The game server registers and sends its heartbeats from then on (see [Server](/R-Type/server/#link-with-the-master-servermastermodule)).

## Routes of a game server: `ServersModule`

A game server is named by its key, sent as `Authorization: Bearer <key>`, never by an id in the path: a key reaches its own server alone. The routes that write answer 401, with `WWW-Authenticate: Bearer`, to a request without a key the master issued; the key is checked before the body is read. Reading the list of the servers, the games or the history of a server needs no key.

| Route | Does | Answers |
|---|---|---|
| `POST /api/servers` | creates a server and its key (see [Creating a game server](#creating-a-game-server-post-apiservers)) | 201, `Location: /api/servers/<id>` and the server with its key; 429 over the quota |
| `PUT /api/servers/me` | replaces what the server of the key registered; the same body twice gives the same server | 200 and the server, its `id` included |
| `PUT /api/servers/me/heartbeat` | replaces the last state of the server, and keeps a sample of its measures | 200 and `{"commands":[]}`, the orders of the admins waiting for the server, none until administration exists; 409 before the server registered |
| `GET /api/servers/{id}/games` | lists the games of the last heartbeat of the server `id`, to anyone (see [Listing the games of a server](#listing-the-games-of-a-server)) | 200 and the games; 404 for an id that names no server |
| `GET /api/servers` | lists the registered servers, a page at a time, to anyone (see [Listing the servers and their history](#listing-the-servers-and-their-history)) | 200 and the servers; 400 for a page out of its bounds |
| `GET /api/servers/{id}/samples` | lists the samples of the server `id`, newest first | 200 and the samples; 404 for an id that names no server, 400 for a page out of its bounds |
| `GET /api/servers/{id}/statuses` | lists the statuses the server `id` held, each with the second it took it, newest first | 200 and the statuses; 404 and 400 as above |

A registration is the state of a server that starts: it forgets the last heartbeat, since the server hosts no game yet. A registration with no `address` takes the address of the TCP peer, since a server often does not know its public address. A sample is written once per second of the clock at most, whatever the rate of the heartbeats, and every sample older than `SAMPLE_RETENTION` (7 days) is erased when a new one is written, of every server.

The bodies are JSON, read and written by `rtype_network_master` (`src/network/Master/`), which `MasterClient` links to send them. A member it does not know is ignored, so a newer server still registers; a member missing, of another type or out of its bounds answers 400, whose `detail` names it by its JSON Pointer (RFC 6901), such as `/games/0/players/1`:

```json
{"name":"Paris 1","udpPort":4242,"gameVersion":"0.4.0","protocolVersion":6,
 "maximumGames":8,"maximumPlayersPerGame":4}
```

```json
{"games":[{"number":1,"name":"GAME 1","state":"waiting","difficulty":"normal","players":["ACE"]}],
 "measures":{"tickP50Milliseconds":1.5,"tickP95Milliseconds":4.25}}
```

| Member | Bounds |
|---|---|
| `name`, `gameVersion` | 1 to 32 bytes of printable UTF-8 (`MAXIMUM_SERVER_NAME_SIZE`, `MAXIMUM_VERSION_SIZE`); the `name` of a game: 1 to 16 bytes (`MAXIMUM_GAME_NAME_SIZE`) |
| `address` | optional, an IPv4 address |
| `udpPort` | 1 to 65535 |
| `maximumGames`, `maximumPlayersPerGame` | 1 to `MAXIMUM_HOSTED_GAMES` (64), 1 to `MAXIMUM_PLAYERS` (4) |
| `games` | as many as the server registered at most; numbers from 1 to its `maximumGames`, none twice |
| `state` | `waiting`, `starting`, `playing`, `finished` or `full`; `full` means the game waits for players and every seat is taken, so it holds as many players as the server registered per game, and `waiting` holds fewer; the other states hold any number |
| `difficulty`, `players` | nicknames of the protocol, 1 to 16 bytes; as many players as the server registered per game at most |
| `measures` | an object; each of `tickP50Milliseconds`, `tickP95Milliseconds`, `receivedBytesPerSecond`, `sentBytesPerSecond`, `lossPercent` (0 to 100) and `averagePingMilliseconds` is a number from 0, or left out when the server does not measure it, never sent as 0 |

## Listing the games of a server

`GET /api/servers/{id}/games` answers, to anyone, the games of the last heartbeat of a server, so a player who picked a server can choose a game in it. The `id` is the one the creation answered. A server that registered and sent no heartbeat since lists no game: `{"games":[]}`, since a registration forgets the last heartbeat. An `id` that is no number, or that no server has, answers 404.

```json
{"games":[{"number":1,"name":"GAME 1","state":"waiting","playerCount":1},
          {"number":2,"name":"GAME 2","state":"inGame","playerCount":2}]}
```

The answer never holds a nickname, a difficulty or a measure: those stay in the heartbeat. The state of a game is the one of its heartbeat, folded to the three a player picks from:

| Heartbeat | Listed as |
|---|---|
| `waiting` | `waiting`: a seat is free |
| `full` | `full`: every seat is taken |
| `starting`, `playing`, `finished` | `inGame`; a game that is over keeps its number and its line |

A read costs one SQL statement and the check of the heartbeat kept, and the route has no quota: the quota of creations is the only limit of the API today. The answer holds at most `MAXIMUM_HOSTED_GAMES` (64) games.

Three things the route does not do:

- **It is not paginated.** The rule of the routes of the master asks `?limit&offset` for a list; this one answers the whole list instead, since no server holds more than `MAXIMUM_HOSTED_GAMES` (64) games, which is the answer's upper bound.
- **It does not age the list.** The games are those of the last heartbeat kept, whatever its date: a server that stopped sending heartbeats keeps listing its games as it last described them, until it registers again. Its status in [the list of the servers](#listing-the-servers-and-their-history) says whether it still beats.
- **It does not hide a heartbeat it can no longer read.** A last heartbeat stored in a form the master no longer reads (for instance one stored before `name` became a member of a game) makes `ServersService::gameListingOf()` throw `InvalidMasterMessageException`. The controller does not catch it, so the exception handler of the HTTP server answers 500 and writes the reason in the journal. The next heartbeat of the server replaces the stored one.

The master and the game servers are deployed together: a game of a heartbeat must carry its `name`, and a master that reads this version refuses with 400 the heartbeat of a server that does not write it.

## Statuses of the game servers: `ServerStatusJob`

The master gives each registered server a status, refreshed by `ServerStatusJob` (`src/master/Servers/Jobs/`) on its own thread, `StatusWatch`, which `ServersModule` starts with the master and stops with it: once at the start, then every `STATUS_REFRESH_INTERVAL` (5 s), whether anyone reads the list or not. `statusOf()` (`src/master/Servers/Services/Helpers/ServerStatusRule.hpp`) gives the first status that applies:

| Status | When |
|---|---|
| `maintenance` | an admin took the server out of service, whatever it sends |
| `offline` | the master heard nothing of it for `OFFLINE_SILENCE` (15 s, `MISSED_HEARTBEATS_BEFORE_OFFLINE` (3) heartbeats of `MASTER_HEARTBEAT_INTERVAL` (5 s)): no heartbeat for that long, or, while it sent none since it registered, no registration |
| `degraded` | heard in time, but the 95th percentile of its ticks reaches `DEGRADED_TICK_P95_MILLISECONDS` (one tick of the simulation, 16.67 ms) or its loss reaches `DEGRADED_LOSS_PERCENT` (5 %) |
| `online` | heard in time and within both bounds; a measure the server does not take counts as within its bound |

- **The bounds are provisional.** They are starting values, to set again once the bandwidth and lag measurements are done; code must read the constants. The game server sends no loss yet (see [Server](/R-Type/server/)), so it is judged on its ticks and its silence only.
- **Timing.** Dates are kept to the second, so a server that stops beating is written offline 14 to 20 s after its last heartbeat. A server starts offline, the default of its column, and its first refresh after its registration writes it online. A server created on the web site and never registered is neither refreshed nor listed.
- **History.** Each change is kept in `status_changes` with its second, and written as the status of its server. A refresh reads the servers, keeps their changes and writes their statuses in one transaction, under the lock of the database: a heartbeat waits for the refresh to end, so no status is computed from a server heard again meanwhile, and a refresh that fails keeps no change and no status. A second change of a server in the same second replaces the first. Changes are kept `STATUS_CHANGE_RETENTION` (7 days, as long as the samples): each refresh erases the older ones, so a server whose status has not changed for 7 days has an empty history, while the list still gives its status. Each change is written in the journal at the `info` level: `server 7 is now offline`. A refresh that fails is written at the `error` level and tried again at the next one.
- **Not written yet.** The status `unreachable`, for a server that does not answer 3 probes over UDP in a row, will come with the probe. Putting a server in maintenance has no route: `ServerStatusService::setMaintenance()` exists, and the administration will call it. A server at the edge of a bound changes status each time a heartbeat crosses it: no delay keeps it in place.

## Listing the servers and their history

Three routes read what the master keeps, to anyone, a page at a time. `GET /api/servers` lists the registered servers by id, each with its registration, the status computed last and the games and players of its last heartbeat (0 and 0 before one):

```json
{"servers":[{"id":7,"name":"Paris 1","address":"203.0.113.7","udpPort":4242,"gameVersion":"0.4.0",
             "protocolVersion":2,"maximumGames":8,"maximumPlayersPerGame":4,
             "status":"online","gameCount":2,"playerCount":3}]}
```

`GET /api/servers/{id}/samples` and `GET /api/servers/{id}/statuses` list the samples of one server and the statuses it held, newest first; a status gives the second it was taken, `since`; a date is in seconds since 1970 UTC, and a measure the server did not take is left out:

```json
{"samples":[{"takenAt":1800000000,"gameCount":2,"playerCount":3,
             "measures":{"tickP95Milliseconds":4.25}}]}
{"statuses":[{"status":"offline","since":1800000000}]}
```

- **A page** is `?limit=<n>&offset=<m>`: `limit` from `MINIMUM_PAGE_SIZE` (1) to `MAXIMUM_PAGE_SIZE` (100), `DEFAULT_PAGE_SIZE` (50) when left out; `offset` from `FIRST_PAGE_OFFSET` (0), the items skipped. Each is a whole number with no sign; anything else answers 400, whose `detail` gives the bounds. A parameter given twice keeps its first value.
- **An id** that is no number, or that no server has, answers 404. A server created and never registered has a history, empty.
- **A read** is one SQL statement of plain columns: no heartbeat is parsed. SQLite steps through the rows an `offset` skips, so a page deep in 7 days of samples costs more than the first one.
- `MasterClient` has no function for these routes yet: the list of servers of the game client will add it.

## The client of the master: `MasterClient`

`MasterClient` (`rtype_network_master_client`, `src/network/Master/Client/`) is what a game server and, to read the games of a server, the game client call: one function per route, each sending the message of its route with the key of `MasterAccess` as Bearer, when it has one.

```cpp
rtype::network::MasterClient master{rtype::network::MasterAccess{
    .url = "http://master.example.org:8080", .key = masterKey}};
master.registerServer(registration);
master.sendHeartbeat(heartbeat);

rtype::network::GameListing listing = master.listGames(serverId);  // no key needed
```

- **Every call waits for its answer**, `MASTER_CONNECTION_TIMEOUT` (5 s) to connect, then `MASTER_TRANSFER_TIMEOUT` (5 s) for each read and write, so it is made away from the game loop. Two threads never call the same client together.
- **An error status** throws `MasterRefusedException`: `status()` and the `detail` of the problem document. **No answer** throws `MasterUnreachableException`.
- **Plain HTTP only:** cpp-httplib is built without TLS, so an `https://` url throws `InvalidMasterUrlException`, and the key crosses the network in clear. The master and its game servers must stay on a local network until the client speaks HTTPS.

## Adding a route

A module listed after `http` in `masterModules()` requires the `MasterHttpServer` while it installs, and adds its route there, before the server opens. Its `dependencies()` names `http`, so the application refuses a list where it comes first. The double of the tests, `RouteAddingModule` (`tests/doubles/`), does it this way:

```cpp
void RouteAddingModule::install(rtype::engine::InstallContext& context) {
  context.require<rtype::master::MasterHttpServer>().add(
      rtype::network::MasterRoute{.method = rtype::network::RouteMethod::Get,
                                  .path = path_},
      []([[maybe_unused]] const rtype::master::ApiRequest& request) {
        return rtype::master::ApiResponse{.status = rtype::master::OK_STATUS,
                                          .body = {}};
      });
}
```

`ServersModule` adds its seven routes the same way, from `MasterRouteConstants.hpp`; a path may name a parameter, as `/servers/:id/games` names `id`, which the route reads in `ApiRequest::pathParameters`, and the query of a path, such as `?limit=20`, reaches the route in `ApiRequest::queryParameters`. The module is laid out like a web backend: `Controllers/` reads the request of each route and writes its answer, `Services/` applies the rules (`Services/Helpers/` for the pure ones: the key and its fingerprint, the rule of the statuses), `Repositories/` holds the only SQL, one class per table, `Models/` the plain structs the layers hand each other, and `Jobs/` the work the module does on its own thread at a fixed interval, `ServerStatusJob`.

## Where to intervene

| You want to… | Look at |
|---|---|
| Add a route of the API | its method and path in `src/network/Master/Routes/MasterRouteConstants.hpp`, its status in `src/master/Http/HttpConstants.hpp`, and the module that answers it, as in [Adding a route](#adding-a-route) |
| Add a table | a migration at the end of `MASTER_MIGRATIONS`, never an edit of one a master already applied |
| Change what a server sends, or its bounds | `src/network/Master/`: `MasterMessageText.cpp` and `MasterMessageConstants.hpp` |
| Change what a creation, a registration or a heartbeat answers | `ServersController` (`src/master/Servers/Controllers/`) |
| Change the quota of creations | `MAXIMUM_CREATIONS_PER_ADDRESS` and `CREATION_WINDOW` in `src/master/Servers/Services/ServersServiceConstants.hpp` |
| Change how long samples and changes of status are kept | `SAMPLE_RETENTION` in `src/master/Servers/Repositories/ServersRepositoryConstants.hpp`; `STATUS_CHANGE_RETENTION` follows it |
| Change when a server is offline or degraded | `OFFLINE_SILENCE`, `DEGRADED_TICK_P95_MILLISECONDS` and `DEGRADED_LOSS_PERCENT` in `src/master/Servers/Services/Helpers/ServerStatusRuleConstants.hpp`, and `statusOf()` next to them |
| Change how often the statuses are refreshed | `STATUS_REFRESH_INTERVAL` in `src/master/Servers/Jobs/ServerStatusJobConstants.hpp` |
| Change how often a game server beats | `MASTER_HEARTBEAT_INTERVAL` in `src/network/Master/Validators/MasterMessageConstants.hpp`, which the master also reads to tell a silent server |
| Change the size of a page of a list | `MINIMUM_PAGE_SIZE`, `DEFAULT_PAGE_SIZE` and `MAXIMUM_PAGE_SIZE` in `src/network/Master/Routes/MasterRouteConstants.hpp` |
| Call the master from the game server | `MasterClient` (`src/network/Master/Client/`) |
| Change what the games of a server are listed as | `listedStateOf()` in `ServersService.cpp` (`src/master/Servers/Services/`) and `GameListing` (`src/network/Master/Messages/`) |
| Change what a game server tells of its games | `hostedGamesOf()` (`src/server/Games/HostedGames.cpp`) |
| Add a setting of the master | the `install()` of the module that reads it, `master.cfg.example` and the table above |
| Change what an error answers | `problemDetailsText()` in `MasterHttpServer.cpp` |
| Add a module | `src/master/<Name>/`, its line in `masterModules()` and its row in [the list of modules](/R-Type/project-layout/#the-list-of-modules) |
