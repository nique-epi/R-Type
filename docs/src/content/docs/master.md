---
title: Master
description: What the master does today, how to start it, and where to add a route, a table or a module.
---

The master is the program that lists the game servers and will direct them: each server registers with a key and tells it, every few seconds, which games it hosts and who plays them; a player will read that list in the client, and an admin will send orders from the web site. It is a mode of the server binary, `r-type_server --mode master`, and it never runs a game: no datagram of a game goes through it, so the players of a server keep playing when the master stops. Why it speaks HTTP and why it is a mode rather than a binary of its own is told in [Architecture](/R-Type/architecture/#master).

This page tells you where to look when you need to change something. It describes what exists today and says plainly what does not exist yet.

| Part | State | Where |
|---|---|---|
| Start and stop: `--mode master`, `master.cfg`, Ctrl-C and SIGTERM | Available | `src/server/main.cpp`, `src/master/LaunchOptions/` |
| HTTP API under `/api`, every error in JSON | Available | `src/master/Http/` |
| Database `r-type_master.db` | Available: keys, servers, samples | `src/master/Storage/` |
| Keys of the game servers, `--new-server-key` | Available | `src/master/ServerKeys/` |
| Registration and heartbeats of the game servers | Available; no server sends them yet | `src/master/Servers/`, `src/network/Master/` |
| The list of the servers and their games, for the client | Not yet written | |
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
| `--new-server-key <name>` | Issues the key of the game server `<name>`, prints it, and starts nothing else (see [Keys of the game servers](#keys-of-the-game-servers-serverkeys)). |
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
| `server-keys` | `ServerKeysModule` | `logger`, `storage` | nothing |
| `http` | `MasterHttpModule` | `logger` | the port, and the thread that listens on it |
| `servers` | `ServersModule` | `logger`, `storage`, `server-keys`, `http` | nothing: its routes run once `http` listens |

The storage starts before the HTTP API, so a database the master cannot open stops it before its port opens, and the API stops before the database closes. `--new-server-key` runs `serverKeyModules()` instead: `logger`, `storage` and `server-keys` only, so no port opens.

## The HTTP API: `MasterHttpServer`

`MasterHttpServer` (`src/master/Http/`) wraps cpp-httplib, which no other file of the master includes. Its routes sit under `/api`, answer `GET` or `PUT`, and answer `application/json`; they run on the threads of cpp-httplib, at least 8 of them, and must not block for long. A route reads an `ApiRequest`: the body, the key of an `Authorization: Bearer <key>` header (RFC 6750, the scheme in any case) and the address of the TCP peer. It answers an `ApiResponse`: a status, a body, the `detail` of a refusal and one header, such as `Location`.

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
| 1 | `server_keys` | each key an admin issued: its name, its fingerprint, the date |
| 2 | `servers` | each registered server, one per key, and the last heartbeat it sent, nicknames included |
| 2 | `heartbeat_samples` | the measures of each heartbeat, at most one per server and per second, kept 7 days |

## Keys of the game servers: `ServerKeys`

An admin issues the key of a game server on the machine of the master, which prints it once:

```bash
./r-type_server --mode master --new-server-key "Paris 1"
```

- **The key** is `SERVER_KEY_BYTES` (32) drawn by `hydro_random_buf()` of libhydrogen, which seeds itself from the system (`/dev/urandom` on Linux once `/dev/random` is ready, `/dev/urandom` on macOS, `RtlGenRandom` on Windows), written as 64 lowercase hexadecimal digits on the standard output, never in the journal.
- **The database keeps its fingerprint only**, `hydro_hash_hash()` of the key in the context `FINGERPRINT_CONTEXT`, so a copy of the file lets nobody register as a server. A key is 256 random bits, so a fast hash is enough: nobody can try every key, unlike every password.
- **A key that is not 64 lowercase hexadecimal digits** is refused before its fingerprint is computed.
- **In this mode** the HTTP module does not run, so a `set port` line of `master.cfg` is reported as an unknown setting; the key is issued all the same.

Revoking a key, and issuing one from the web site, are not written yet.

## Routes of a game server: `ServersModule`

A game server is named by its key, sent as `Authorization: Bearer <key>`, never by an id in the path: a key reaches its own server alone. Both routes answer 401, with `WWW-Authenticate: Bearer`, to a request without a key the master issued.

| Route | Does | Answers |
|---|---|---|
| `PUT /api/servers/me` | registers the server of the key, or updates it; the same body twice gives the same server | 201 and `Location: /api/servers/<id>` the first time, 200 after; both carry the server, its `id` included |
| `PUT /api/servers/me/heartbeat` | replaces the last state of the server, and keeps a sample of its measures | 200 and `{"commands":[]}`, the orders of the admins waiting for the server, none until administration exists; 404 before any registration |

A registration is the state of a server that starts: it forgets the last heartbeat, since the server hosts no game yet. A registration with no `address` takes the address of the TCP peer, since a server often does not know its public address. A sample is written once per second of the clock at most, whatever the rate of the heartbeats, and every sample older than `SAMPLE_RETENTION` (7 days) is erased when a new one is written, of every server.

The bodies are JSON, read and written by `rtype_network_master` (`src/network/Master/`), which the server will link to send them. A member it does not know is ignored, so a newer server still registers; a member missing, of another type or out of its bounds answers 400, whose `detail` names it by its JSON Pointer (RFC 6901), such as `/games/0/players/1`:

```json
{"name":"Paris 1","udpPort":4242,"gameVersion":"0.4.0","protocolVersion":2,
 "maximumGames":8,"maximumPlayersPerGame":4}
```

```json
{"games":[{"number":1,"state":"waiting","difficulty":"normal","players":["ACE"]}],
 "measures":{"tickP50Milliseconds":1.5,"tickP95Milliseconds":4.25}}
```

| Member | Bounds |
|---|---|
| `name`, `gameVersion` | 1 to 32 bytes of printable UTF-8 (`MAXIMUM_SERVER_NAME_SIZE`, `MAXIMUM_VERSION_SIZE`) |
| `address` | optional, an IPv4 address |
| `udpPort` | 1 to 65535 |
| `maximumGames`, `maximumPlayersPerGame` | 1 to `MAXIMUM_HOSTED_GAMES` (64), 1 to `MAXIMUM_PLAYERS` (4) |
| `games` | as many as the server registered at most; numbers from 1 to its `maximumGames`, none twice |
| `state` | `waiting`, `starting`, `playing` or `finished` |
| `difficulty`, `players` | nicknames of the protocol, 1 to 16 bytes; as many players as the server registered per game at most |
| `measures` | an object; each of `tickP50Milliseconds`, `tickP95Milliseconds`, `receivedBytesPerSecond`, `sentBytesPerSecond`, `lossPercent` (0 to 100) and `averagePingMilliseconds` is a number from 0, or left out when the server does not measure it, never sent as 0 |

## Adding a route

A module listed after `http` in `masterModules()` requires the `MasterHttpServer` while it installs, and adds its route there, before the server opens. Its `dependencies()` names `http`, so the application refuses a list where it comes first. The double of the tests, `RouteAddingModule` (`tests/doubles/`), does it this way:

```cpp
void RouteAddingModule::install(rtype::engine::InstallContext& context) {
  context.require<rtype::master::MasterHttpServer>().addGet(
      path_, []([[maybe_unused]] const rtype::master::ApiRequest& request) {
        return rtype::master::ApiResponse{.status = rtype::master::OK_STATUS,
                                          .body = {}};
      });
}
```

`ServersModule` adds its two routes the same way, and keeps the SQL of its tables in `ServerRepository` alone: a route reads its request in a controller, applies its rules in a service, and writes through a repository.

## Where to intervene

| You want to… | Look at |
|---|---|
| Add a route of the API | a module of its own, as in [Adding a route](#adding-a-route); the route and its status in `src/master/Http/HttpConstants.hpp` |
| Add a table | a migration at the end of `MASTER_MIGRATIONS`, never an edit of one a master already applied |
| Change what a server sends, or its bounds | `src/network/Master/`: `MasterMessageText.cpp` and `MasterMessageConstants.hpp` |
| Change what a registration or a heartbeat answers | `ServersController` (`src/master/Servers/`) |
| Change how long samples are kept | `SAMPLE_RETENTION` in `src/master/Servers/ServersConstants.hpp` |
| Add a setting of the master | the `install()` of the module that reads it, `master.cfg.example` and the table above |
| Change what an error answers | `problemDetailsText()` in `MasterHttpServer.cpp` |
| Add a module | `src/master/<Name>/`, its line in `masterModules()` and its row in [the list of modules](/R-Type/project-layout/#the-list-of-modules) |
