---
title: Master
description: What the master does today, how to start it, and where to add a route, a table or a module.
---

The master is the program that lists the game servers and will direct them: a host creates its server on the master and gets its key, then the server registers with that key and tells the master, every few seconds, which games it hosts and who plays them; on the game server, the supervisor does it once for all its games, which never talk to the master; a player will read that list in the client, and an admin will send orders from the web site. It is a mode of the server binary, `r-type_server --mode master`, and it never runs a game: no datagram of a game goes through it, so the players of a server keep playing when the master stops. Why it speaks HTTP and why it is a mode rather than a binary of its own is told in [Architecture](/R-Type/architecture/#master).

This page tells you where to look when you need to change something. It describes what exists today and says plainly what does not exist yet.

| Part | State | Where |
|---|---|---|
| Start and stop: `--mode master`, `master.cfg`, Ctrl-C and SIGTERM | Available | `src/server/main.cpp`, `src/master/LaunchOptions/` |
| HTTP API under `/api`, every error in JSON | Available | `src/master/Http/` |
| Database `r-type_master.db` | Available: servers, samples, changes of status | `src/master/Storage/` |
| Creation of a game server and its key, `POST /api/servers` | Available | `src/master/Servers/` |
| Registration and heartbeats of the game servers | Available; sent by the module `master` of the game server | `src/master/Servers/`, `src/network/Master/`, `src/server/Master/` |
| The client of the master, `MasterClient` | Available | `src/network/Master/Client/` |
| The games of a server, for the client, `GET /api/servers/{id}/games` | Available | `src/master/Servers/`, `src/network/Master/` |
| The statuses of the servers, their list and their history, `GET /api/servers` | Available; the route that puts a server in maintenance is not written | `src/master/Servers/`, `src/network/Master/` |
| The probe of the UDP port of each game server, module `probe` | Available | `src/master/Probe/`, `src/master/Servers/`, `src/server/Master/` |
| The web site, a Next.js server of its own that calls the API | Available: the pages of the design in French and English (home, download, sector map, a server, leaderboard, wiki, team); the leaderboard reads `GET /api/leaderboard` | `web/` |
| The results of the games and the global leaderboard, `PUT /api/servers/me/results/{gameId}` and `GET /api/leaderboard` | Available; a game server sends its vanilla games after its heartbeats | `src/master/Leaderboard/`, `src/network/Master/` |
| The master on AWS, installed from each release | Available; see [Deploying the master](/R-Type/deploy-master/) | `infra/`, `.github/workflows/deploy-master.yaml` |
| A pool of game servers that grows from the list of the master, for a LAN | Available; not run yet; see [Game servers for a LAN](/R-Type/deploy-game-servers/) | `infra/game-servers/`, `infra/terraform-game/` |
| Administration, accounts | Not yet written | |

## Starting the master

`--mode master` comes first, before every other option; `r-type_server` alone, or `--mode supervisor`, runs the game server, its supervisor, and `--mode game` is the game process a supervisor starts (see [Server](/R-Type/server/#the-three-roles-supervisor-game-and-master)). `chooseServerMode()` (`src/server/LaunchOptions/`) reads the mode, then `MasterLaunchOptions` (`src/master/LaunchOptions/`) reads the rest in one pass, the logging options included (see [Logging](/R-Type/logging/#choosing-the-level-and-the-output-at-launch)).

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

The master reads `master.cfg`, in the folder it is started from, between the install and the start of its modules, in the language of `server.cfg` (see [Configuration commands](/R-Type/commands/)). A missing file is not an error. `master.cfg.example`, at the root of the repository, lists every setting with its default.

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
| `probe` | `MasterProbeModule` | `logger` | a UDP socket on a port the system picks, and ProbeWire, the thread that sends the probes on it |
| `http` | `MasterHttpModule` | `logger` | the port, and the thread that listens on it |
| `servers` | `ServersModule` | `logger`, `storage`, `probe`, `http` | `StatusWatch`, the thread of `ServerStatusJob`; its routes run once `http` listens |
| `leaderboard` | `LeaderboardModule` | `storage`, `http`, `servers` | nothing; its routes run once `http` listens |

The storage starts before the HTTP API, so a database the master cannot open stops it before its port opens, and the API stops before the database closes. The probes start before the HTTP API too, so a heartbeat a route accepts always has its probe sent, and they stop after the routes.

## The HTTP API: `MasterHttpServer`

`MasterHttpServer` (`src/master/Http/`) holds a `network::HttpServer` (`src/network/Http/`, see [Network](/R-Type/network/#http-server-httpserver)), which wraps cpp-httplib and which the supervisor also uses to serve its content pack; no file of the master includes cpp-httplib. Its routes sit under `/api`, answer `GET`, `POST` or `PUT`, and answer `application/json`; each is declared once, with its method and path, in `MasterRouteConstants.hpp` (`src/network/Master/Routes/`), which the master and `MasterClient` both read; they run on the threads of cpp-httplib, at least 8 of them, and must not block for long. A route reads a `network::ApiRequest`: the body, the key of an `Authorization: Bearer <key>` header (RFC 6750, the scheme in any case) and the address of the client. It answers a `network::ApiResponse`: a status, a body, the `detail` of a refusal, one header, such as `Location`, and the media type of the body, `application/json` unless the route sets another.

`GET /api/health` answers 200 with the version of the master, `MASTER_VERSION`, the version of `project()` in the root `CMakeLists.txt`:

```json
{"version":"1.1.0"}
```

- **Errors:** an answer of 400 or more without a body gets a problem document of RFC 9457, `application/problem+json`, whose `title` is the phrase of its status, with the `detail` of the route when it gives one: `{"status":404,"title":"Not Found"}` for a path no route answers.
- **The address of the client** is the one of the TCP connection, except for a request from `127.0.0.1`, the reverse proxy on the machine: its client is the last address of its `X-Forwarded-For` header, the one the proxy saw (`clientAddressOf()`, `src/network/Http/Server/`). A client that reaches the master from elsewhere cannot name another address, and Caddy drops the `X-Forwarded-For` a client sends before it writes its own. The supervisor serves its content pack with the same server, and reads the TCP peer only.
- **A route that throws** answers 500, and its reason goes to the journal only, never to the client.
- **A body over 64 KiB** (`MAX_REQUEST_BODY_BYTES`) sent to a route answers 413 without being kept whole; cpp-httplib would keep up to 100 MiB. A path no route answers gets 404 without its body being read.
- **One master per port:** by default cpp-httplib sets `SO_REUSEPORT`, which lets a second program bind the port of the first with no error. The master sets `SO_REUSEADDR` instead (`SO_EXCLUSIVEADDRUSE` on Windows), so a second master on the same port stops at its start, with an error naming the port.
- **No encryption of its own:** the master listens in plain HTTP on every IPv4 interface (`ANY_IPV4_ADDRESS`), for a local network. On the Internet, Caddy in front of it answers HTTPS, as the deployment on AWS does (see [Deploying the master](/R-Type/deploy-master/)).

## The web site

The web site is a program of its own: a Next.js server, in `web/` (see [Web site](/R-Type/technology-choices/#web-site-nextjs) for its tools). The browser only talks to that server, by default on port 3000; the server renders each page and calls the API of the master itself, at `MASTER_URL` (`https://master.rtype.app` by default). The browser never calls the master, so the master needs no CORS, and the master serves its API only.

- **Each request of a page that lists servers asks the master again**: the home page, the sector map, a server and the leaderboard are rendered at request time.
- **A master that does not answer the list** gives the five example servers of the design (`SAMPLE_SERVERS`), and each page that shows them says they are examples. A master that answers an empty list gives an empty page: only silence gives examples.
- **The client of the master is server-only** (`web/src/lib/api/`): its code and `MASTER_URL` never reach the browser.
- **The download page reads the latest release of the GitHub repository** (`web/src/lib/github/`), `nique-epi/R-Type` unless `GITHUB_REPOSITORY` says otherwise: the version, the date, the size and the link of the installer of each system, and the last three entries of its notes. Next.js keeps the answer 10 minutes (`GITHUB_TOKEN` raises the 60 requests an hour per address the API gives without one). When GitHub does not answer, the pages show no version and link to the page of the releases.
- **Every page lives under a language**, `/fr` or `/en`: `web/src/proxy.ts` sends a path with none to the language the browser prefers (`Accept-Language`), French when it names none the site speaks.

### The pages of the site

| Path under `/fr` or `/en` | Shows | Reads |
|---|---|---|
| `/` | the hero, the servers with free seats, the Bydos, the features, the notes of the latest release | the list of servers, the latest release |
| `/download` | the installer of the system of the reader, its steps, the other files of the release | the latest release |
| `/servers` | the sector map, read only: a seat is taken in the game, so the map has no Join and no quick play | the list of servers, each with its games and its last ping |
| `/servers/{id}` | the games of a server, its figures and the pilots of its last 100 samples (the master keeps one sample a second, a page of the list holds 100); a server created and not registered yet shows a waiting notice | the list, `GET /api/servers/{id}/samples` |
| `/leaderboard` | the leaderboard screen of the game, by server or for everyone; the scope and the filters are in the address (`?scope=server&server=7&difficulty=hard&pilots=3`) and the page reads the ten best players again at each address | the leaderboard, and the list of servers for the first tab |
| `/wiki/{keys,enemies,accessibility,host}` | the controls, the bestiary, the pilot palettes, the guide to host a server | nothing |
| `/team` | the four members and their GitHub | nothing |

- **The ping of a star** is the mean round trip of the players of its server at its last sample (`averagePingMilliseconds`), since the site cannot measure one from the browser. A server with no player has none, and is placed at `UNMEASURED_PING_MILLISECONDS` (75).
- **The statuses of the master are folded to three**: `online` and `degraded` stay, `offline`, `unreachable` and `maintenance` are drawn offline, since none of them can be played on.
- **The key of a server never leaves the page that created it**: the server action returns it to that page and logs nothing, and the hosting page refuses a name the form would not send (empty, or longer than 32 characters) without asking the master.

### The components of the site

`web/src/components/` holds every component the design of the site needs: the home page, the downloads, the sector map, a server, hosting, the leaderboard, the wiki and the team. A component takes its data and its words as props and never calls the master, so a block reads the API, then hands the answer down.

| Folder | Holds |
|---|---|
| `primitives/` | the pieces with no data, one folder per kind: `actions/` (`Button` and `ArrowLink`, a link when given an `href`, a button otherwise), `inputs/` (`Tabs`, `TextField`), `status/` (`StatusBadge`, `Tag`), `surfaces/` (`Dialog` on the native `<dialog>`, `Table`), `text/`, `layout/`, `icons/` |
| `motion/`, `sky/`, `sprites/` | the animations, the star layers and the planet, and `Sprite`, which draws one frame of a sheet of `web/public/sprites/` at a whole scale |
| `layout/`, `game/` | the header, the menu and the hero of a page; a game screen of 960 × 540, which scales to the width it is given, with its frames and chips |
| `home/`, `download/`, `servers/`, `ranking/`, `wiki/`, `team/` | the sections of each page, such as `SectorMap` and `RankingScreen` |
| `blocks/` | the bigger pieces a page is made of, built from the ones above: a block reads its own words (`getDictionary()`) and its own data (`web/src/hooks/api/`), so a page calls it with a language and nothing else; one folder per page (`blocks/home/`, `blocks/servers/`…), and `blocks/shell/` for what every page sits in |

- **Every animation goes through `useMotion()`** (`web/src/hooks/`), which runs nothing when the system asks for reduced motion, so the page then shows each element in its final place. `SmoothScroll` drives Lenis from the ticker of GSAP, so ScrollTrigger reads the scroll the page draws.
- **The words come from a dictionary**, French or English (`web/src/i18n/dictionaries/`), given by `getDictionary(locale)`. Each component names the words it needs in a label type, and `Dictionary` gathers them, so a word missing from a language fails the type check. A template holds `{name}` holes, filled by `format()`, or by `formatNodes()` when a word is set in bold.
- **The colors are the palette of the client** (`SKY_COLOR`, `HUD_CYAN`…), as Tailwind tokens in `web/src/styles/globals.css`: `bg-sky`, `text-hud-cyan`. `md:` starts at 861px, where the layouts stop stacking.

```tsx
const words = getDictionary('fr');

return (
  <>
    <StatusBadge status="degraded">{words.status.degraded}</StatusBadge>
    <Button href="/download" variant="solid">
      {words.header.download}
    </Button>
  </>
);
```

## The database: `MasterStorageModule`

The storage module opens its file with `rtype_storage` (`openDatabaseFile()`, see [Project layout](/R-Type/project-layout/#storage)): write-ahead log, foreign keys on, and the migrations of `MASTER_MIGRATIONS` (`src/master/Storage/MasterStorageConstants.hpp`) the file does not hold yet. A file it cannot open, or one written by a newer master, stops the start with an error naming the file. It provides the connection as a `MasterDatabase`, which the threads of the HTTP server share behind one lock: most requests hold it for a few statements of one row each, and a read of the leaderboard holds it while it ranks every row of one difficulty and one number of players, so heartbeats wait behind it (see [The global leaderboard](#the-global-leaderboard-leaderboardmodule)).

| Version | Table | Holds |
|---|---|---|
| 1 | `servers` | each created server: its name, the fingerprint of its key, the address and the date of its creation; once it registered, what it registered and the last heartbeat it sent, nicknames included |
| 1 | `heartbeat_samples` | the measures of each heartbeat, at most one per server and per second, kept 7 days |
| 2 | `servers` | the status computed last, whether an admin put the server in maintenance, and the games, players, tick p95 and loss of the last heartbeat, written with it so the list and the statuses parse no JSON; the upgrade of a file of version 1 reads them once from the last heartbeat it kept, and leaves them empty for a text that is no heartbeat |
| 2 | `status_changes` | each change of status of each server, at most one per server and per second, kept 7 days |
| 3 | `servers` | the secret each server registered with, which its probes carry, and the second of its last valid answer to a probe; both empty for a server registered before |
| 4 | `game_results` | a row per player of each vanilla game a server reported: its server, the id of the game, the nickname, the account, the score, the date, the difficulty and the number of players; the key is the server, the game and the nickname, compared without case |

## Creating a game server: `POST /api/servers`

A host creates its server on the web site, whose hosting page calls `POST /api/servers` with the name players will read (a call can also be made by hand or by `MasterClient::createServer()`). The master answers 201, `Location: /api/servers/<id>`, and the server with its key:

```json
{"id":7,"name":"Paris 1","key":"rtk_<64 hexadecimal digits>"}
```

- **The key is shown this once.** It is `rtk_`, so a key that leaks in a log or a repository is recognized, then `SERVER_KEY_BYTES` (32) drawn by `hydro_random_buf()` of libhydrogen, which seeds itself from the system (`/dev/urandom` on Linux once `/dev/random` is ready, `/dev/urandom` on macOS, `RtlGenRandom` on Windows), in lowercase hexadecimal: 68 characters. It never goes in a URL or in the journal.
- **The database keeps its fingerprint only**, `hydro_hash_hash()` of the key in the context `FINGERPRINT_CONTEXT`, so a copy of the file lets nobody act as a server. A key is 256 random bits, so a fast hash is enough: nobody can try every key, unlike every password.
- **One address creates `MAXIMUM_CREATIONS_PER_ADDRESS` (5) servers per `CREATION_WINDOW` (1 hour) at most**; the next one answers 429. The count and the insert are one transaction, so requests sent at once cannot pass the quota together. The address is the one of the TCP peer: behind a reverse proxy, every host shares the quota of the proxy.
- **Anyone who reaches the master may create a server** until accounts exist: the web site will then ask for a signed-in host. Revoking a key, and giving it an expiry, are not written yet.

The host then writes the key in the `server.cfg` of that server, as the setting `master-key`, next to `master-url`; `server.cfg` is never committed, only `server.cfg.example`, with an empty key. The supervisor of the game server registers and sends its heartbeats from then on (see [Server](/R-Type/server/#link-with-the-master-servermastermodule)). One supervisor is one server on the master: it registers its own UDP port, `maximumGames` 8 (`MAXIMUM_RUNNING_GAMES`), and in each heartbeat the games it runs, numbered by place from 1 to 8 and named `GAME 1` to `GAME 8`; a place freed by a game that ended is given to the next game started, so a number names a place, not one game for ever. Its `measures` are the median and the 95th percentile of the ticks of its slowest game and the mean round trip of its players, each game counting as many times as it has players; it sends no traffic and no loss.

## Routes of a game server: `ServersModule`

A game server is named by its key, sent as `Authorization: Bearer <key>`, never by an id in the path: a key reaches its own server alone. The routes that write answer 401, with `WWW-Authenticate: Bearer`, to a request without a key the master issued; the key is checked before the body is read. Reading the list of the servers, the games or the history of a server needs no key.

| Route | Does | Answers |
|---|---|---|
| `POST /api/servers` | creates a server and its key (see [Creating a game server](#creating-a-game-server-post-apiservers)) | 201, `Location: /api/servers/<id>` and the server with its key; 429 over the quota |
| `PUT /api/servers/me` | replaces what the server of the key registered; the same body twice gives the same server | 200 and the server, its `id` included |
| `PUT /api/servers/me/heartbeat` | replaces the last state of the server, keeps a sample of its measures, records the answer to a probe it carries, and sends the server a probe (see [Probing the game servers](#probing-the-game-servers-masterprobemodule)) | 200 and `{"commands":[]}`, the orders of the admins waiting for the server, none until administration exists; 409 before the server registered, or for a server registered before the probes |
| `PUT /api/servers/me/results/{gameId}` | keeps the result of a vanilla game of the server (see [The global leaderboard](#the-global-leaderboard-leaderboardmodule)), answered by the module `leaderboard` | 204; 400 for a game that was not vanilla |
| `GET /api/servers/{id}/games` | lists the games of the last heartbeat of the server `id`, to anyone (see [Listing the games of a server](#listing-the-games-of-a-server)) | 200 and the games; 404 for an id that names no server |
| `GET /api/servers` | lists the registered servers, a page at a time, to anyone (see [Listing the servers and their history](#listing-the-servers-and-their-history)) | 200 and the servers; 400 for a page out of its bounds |
| `GET /api/servers/{id}/samples` | lists the samples of the server `id`, newest first | 200 and the samples; 404 for an id that names no server, 400 for a page out of its bounds |
| `GET /api/servers/{id}/statuses` | lists the statuses the server `id` held, each with the second it took it, newest first | 200 and the statuses; 404 and 400 as above |

A registration is the state of a server that starts: it forgets the last heartbeat, since the server hosts no game yet. A registration with no `address` takes the address of the TCP peer, since a server often does not know its public address; a supervisor declares one with its `public-address` setting, which a supervisor on the machine of the master needs, or it is listed at `127.0.0.1`. A sample is written once per second of the clock at most, whatever the rate of the heartbeats, and every sample older than `SAMPLE_RETENTION` (7 days) is erased when a new one is written, of every server.

The bodies are JSON, read and written by `rtype_network_master` (`src/network/Master/`), which `MasterClient` links to send them. A member it does not know is ignored, so a newer server still registers; a member missing, of another type or out of its bounds answers 400, whose `detail` names it by its JSON Pointer (RFC 6901), such as `/games/0/players/1`:

```json
{"name":"Paris 1","udpPort":4242,"gameVersion":"0.4.0","protocolVersion":7,
 "maximumGames":8,"maximumPlayersPerGame":4,"probeSecret":6510615555426900570}
```

```json
{"games":[{"number":1,"name":"GAME 1","state":"waiting","difficulty":"normal","lives":3,
            "friendlyFire":false,"players":["ACE"]}],
 "difficulties":["easy","normal","hard"],
 "measures":{"tickP50Milliseconds":1.5,"tickP95Milliseconds":4.25},"probeToken":4242424242}
```

| Member | Bounds |
|---|---|
| `name`, `gameVersion` | 1 to 32 bytes of printable UTF-8 (`MAXIMUM_SERVER_NAME_SIZE`, `MAXIMUM_GAME_VERSION_SIZE`); the `name` of a game: 1 to 16 bytes (`MAXIMUM_GAME_NAME_SIZE`) |
| `address` | optional, an IPv4 address |
| `udpPort` | 1 to 65535 |
| `maximumGames`, `maximumPlayersPerGame` | 1 to `MAXIMUM_HOSTED_GAMES` (64), 1 to `MAXIMUM_PLAYERS` (4) |
| `games` | as many as the server registered at most; numbers from 1 to its `maximumGames`, none twice |
| `state` | `waiting`, `starting`, `playing`, `finished` or `full`; `full` means the game waits for players and every seat is taken, so it holds as many players as the server registered per game, and `waiting` holds fewer; the other states hold any number |
| `difficulty`, `players` | nicknames of the protocol, 1 to 16 bytes; as many players as the server registered per game at most |
| `lives`, `friendlyFire` | the rules of a game: 1 to 255 lives (`LOWEST_GAME_LIVES`, `HIGHEST_GAME_LIVES`, what one byte holds; a server accepts fewer when a game is created), and a boolean |
| `difficulties` | the tables of the waves file of the server, the difficulties a new game may be played at: 1 to `MAXIMUM_DIFFICULTIES` (16) nicknames of the protocol, none twice |
| `probeSecret` | 1 to 9223372036854775807 (2^63 − 1, what a column `INTEGER` of SQLite holds); required in a registration, and never written in a list |
| `probeToken` | optional, 1 to 18446744073709551615 (2^64 − 1): the token of the last probe, sent once |
| `measures` | an object; each of `tickP50Milliseconds`, `tickP95Milliseconds`, `receivedBytesPerSecond`, `sentBytesPerSecond`, `lossPercent` (0 to 100) and `averagePingMilliseconds` is a number from 0, or left out when the server does not measure it, never sent as 0 |

## Listing the games of a server

`GET /api/servers/{id}/games` answers, to anyone, the games of the last heartbeat of a server, so a player who picked a server can choose a game in it. The `id` is the one the creation answered. A server that registered and sent no heartbeat since lists no game and no difficulty: `{"difficulties":[],"games":[]}`, since a registration forgets the last heartbeat. An `id` that is no number, or that no server has, answers 404.

```json
{"difficulties":["easy","normal","hard"],
 "games":[{"number":1,"name":"GAME 1","state":"waiting","playerCount":1,
            "difficulty":"normal","lives":3,"friendlyFire":false},
           {"number":2,"name":"GAME 2","state":"inGame","playerCount":2,
            "difficulty":"hard","lives":5,"friendlyFire":true}]}
```

Each game carries its rules, so a player sees them before it joins, and `difficulties` gives the ones a new game of that server may be played at, so a client offers only those. The answer never holds a nickname or a measure: those stay in the heartbeat. The state of a game is the one of its heartbeat, folded to the three a player picks from:

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

The master and the game servers are deployed together: a game of a heartbeat must carry its `name`, its `lives` and its `friendlyFire`, and the heartbeat its `difficulties`; a master that reads this version refuses with 400 the heartbeat of a server that does not write them.

## Statuses of the game servers: `ServerStatusJob`

The master gives each registered server a status, refreshed by `ServerStatusJob` (`src/master/Servers/Jobs/`) on its own thread, `StatusWatch`, which `ServersModule` starts with the master and stops with it: once at the start, then every `STATUS_REFRESH_INTERVAL` (5 s), whether anyone reads the list or not. `statusOf()` (`src/master/Servers/Services/Helpers/ServerStatusRule.hpp`) gives the first status that applies:

| Status | When |
|---|---|
| `maintenance` | an admin took the server out of service, whatever it sends |
| `offline` | the master heard nothing of it for `OFFLINE_SILENCE` (15 s, `MISSED_HEARTBEATS_BEFORE_OFFLINE` (3) heartbeats of `MASTER_HEARTBEAT_INTERVAL` (5 s)): no heartbeat for that long, or, while it sent none since it registered, no registration |
| `unreachable` | heard in time, but no probe got a valid answer for `PROBE_ANSWER_SILENCE` (15 s), or none since it registered: its UDP port cannot be reached (see [Probing the game servers](#probing-the-game-servers-masterprobemodule)) |
| `degraded` | heard in time, but the 95th percentile of its ticks reaches `DEGRADED_TICK_P95_MILLISECONDS` (one tick of the simulation, 16.67 ms) or its loss reaches `DEGRADED_LOSS_PERCENT` (5 %) |
| `online` | heard in time and within both bounds; a measure the server does not take counts as within its bound |

- **The bounds are provisional.** They are starting values, to set again once the bandwidth and lag measurements are done; code must read the constants. The game server sends no loss yet (see [Server](/R-Type/server/)), so it is judged on its ticks and its silence only.
- **Timing.** Dates are kept to the second, so a server that stops beating is written offline 14 to 20 s after its last heartbeat. A server starts offline, the default of its column; its first refresh after its registration writes it unreachable, and the first after a heartbeat that answers a probe online. A server created on the web site and never registered is neither refreshed nor listed.
- **History.** Each change is kept in `status_changes` with its second, and written as the status of its server. A refresh reads the servers, keeps their changes and writes their statuses in one transaction, under the lock of the database: a heartbeat waits for the refresh to end, so no status is computed from a server heard again meanwhile, and a refresh that fails keeps no change and no status. A second change of a server in the same second replaces the first. Changes are kept `STATUS_CHANGE_RETENTION` (7 days, as long as the samples): each refresh erases the older ones, so a server whose status has not changed for 7 days has an empty history, while the list still gives its status. Each change is written in the journal at the `info` level: `server 7 is now offline`. A refresh that fails is written at the `error` level and tried again at the next one.
- **Not written yet.** Putting a server in maintenance has no route: `ServerStatusService::setMaintenance()` exists, and the administration will call it. A server at the edge of a bound changes status each time a heartbeat crosses it: no delay keeps it in place.

## Probing the game servers: `MasterProbeModule`

A server can beat over HTTP while its UDP port, the one players join, is closed by a firewall or a box. The master checks that port with a probe over UDP, on the model of the master of DDNet, and lists as `unreachable` a server whose port does not answer:

1. The server registers with a `probeSecret`, drawn once when it starts (`randomProbeSecret()`, `src/server/Master/`).
2. After each heartbeat it accepts, the master sends a `MasterProbe` to the address and UDP port of the registration: that secret and a token (see [Protocol](/R-Type/protocol/#probe-of-the-master)).
3. The server keeps the token of a probe that carries its secret, and sends it once, as `probeToken`, in its next heartbeat.
4. A heartbeat whose token is valid records its second as the last answer of the server, in the same transaction as the heartbeat.

- **The master keeps nothing per probe.** The token is the `CookieIssuer` of the handshake (see [Network](/R-Type/network/#connection-cookies-cookieissuer)) with `PROBE_TOKEN_WINDOWS`: windows of `MASTER_HEARTBEAT_INTERVAL` (5 s), `UNANSWERED_PROBES_BEFORE_UNREACHABLE` (3) of them accepted, so a token stays valid 10 to 14 s after it was issued. Its key is drawn when the master starts: a token issued before a restart is refused after it, so a server may be listed unreachable until a heartbeat brings back a token of the new key.
- **The module `probe`** (`src/master/Probe/`), required, provides the `IProbeSender` the routes hand each probe to. It sends from a UDP socket bound to a port the system picks, which it never reads, on ProbeWire, a thread of its own (`NetworkContext::runUntilStopped()`). `ProbeThrottle` lets at most one probe a `PROBE_MINIMUM_GAP` (1 s) go to one address and port, so nobody who registers servers at the address of another makes that server ban the master.
- **Timing.** A server that starts is unreachable until its first heartbeat after a probe, 5 to 10 s after it registered: its registration forgets its last answer, so no player joins it before the confirmation. A server whose port closes is written unreachable 15 to 20 s after the last heartbeat that brought back a valid token. One lost probe never changes the status; two in a row may, as two lost heartbeats may make a server offline.
- **A server registered before the probes** has no secret: its heartbeat answers 409, so it registers again with one.
- **What the master trusts.** The probe goes to the address of the registration, the one the server declared or the one its registration came from. A host may declare another address than its own: the master then sends 41 bytes there, at most once a second, fewer than the heartbeat that made it send them.

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
- **A read** of the list is one SQL statement, and a read of a history two: one checks that the server exists, then one reads the page. Both read plain columns, so no heartbeat is parsed. SQLite steps through the rows an `offset` skips, so a page deep in 7 days of samples costs more than the first one.
- `MasterClient::listServers()` reads the list for the server map of the game client (see [The client of the master](#the-client-of-the-master-masterclient)); the histories have no function yet.

## The global leaderboard: `LeaderboardModule`

The leaderboard module (`src/master/Leaderboard/`) gathers the results of every server, so a player can measure up to the players of all servers. A game server reports each vanilla game that ended, after its heartbeats, until the master keeps it (see [Server](/R-Type/server/#link-with-the-master-servermastermodule)), and anyone reads the leaderboard, of one difficulty and one number of players, or of every one of them when the query names none.

| Route | Does | Answers |
|---|---|---|
| `PUT /api/servers/me/results/{gameId}` | keeps a row per player of the game the server of the key drew `gameId` for, in table `game_results` | 204; 401 without a valid key; 400 for an id that is not 32 lowercase hexadecimal digits (`GAME_ID_SIZE`), a body out of its bounds, or a game that was not vanilla |
| `GET /api/leaderboard?difficulty=normal&playerCount=2` | the best score of each player, by rank, a page at a time, to anyone; `difficulty` and `playerCount` each filter only when given, so a query without them reads every game; `server=<id>` reads one server, `player=<nickname>` adds the row of that player | 200; 400 when a parameter given is out of its bounds, an empty `difficulty` included |

```json
{"playedAt":1800000000,"difficulty":"normal","playerCount":2,"vanilla":true,
 "players":[{"nickname":"ACE","score":12000},{"nickname":"R9","accountId":"account-42","score":800}]}
```

The answer to `GET /api/leaderboard?difficulty=normal&playerCount=2&limit=1&player=R9`, when this game is the only one of that leaderboard:

```json
{"scores":[{"rank":1,"nickname":"ACE","score":12000,"playedAt":1800000000,"serverId":7,"serverName":"Paris 1"}],
 "player":{"rank":2,"nickname":"R9","score":800,"playedAt":1800000000,"serverId":7,"serverName":"Paris 1"}}
```

- **The route is a `PUT` on the id the server drew**, since the server, not the master, names the game. A game is kept whole and once per server: a result sent again changes nothing, even with other players or scores, and answers 204 as the first time, so a server may send again whatever it is not sure arrived. A second send never replaces or completes the first.
- **The master keeps vanilla games only**: a body with `"vanilla":false` answers 400, and a game that is not vanilla stays in the leaderboard of its server. The master cannot check a score: it trusts the server an admin gave a key to.
- **A row keeps its server**, so `server=<id>` reads one server, each row of the answer names the server of its game, and the rows of a server can be erased with it.
- **One player, one row**: an account is one player under any nickname, its id compared as written; a guest is one player per nickname, whatever the server. Nicknames are compared as a game server compares them (`nicknameKey()`, SQLite's `NOCASE`): ASCII letters without case, every other character as it is, so `ACE` and `ace` are one guest, `ÉLODIE` and `élodie` two. Among the rows of a player, its best score counts, the earliest game first when two are equal. Equal scores share a rank, and the next rank skips them (1, 1, 3).
- **`player`** names a nickname, compared the same way; the row of the player who played under it in any game of that leaderboard comes in `player` wherever it ranks, under the nickname of its best score, and `player` is left out when it names no one with a score there. A page is `limit` and `offset`, as for [the list of the servers](#listing-the-servers-and-their-history).
- **Bounds of a result**: `playedAt` in seconds since 1970 UTC, from 0 to 2^63 − 1; `difficulty` and each `nickname`, nicknames of the protocol; `playerCount`, the players when the game started, 1 to `MAXIMUM_PLAYERS` (4); 1 to 4 `players`, no more than `playerCount`, since a game server lets nobody in once a game runs, and no nickname twice; `accountId`, left out for a guest, else 1 to `MAXIMUM_ACCOUNT_ID_SIZE` (64) bytes of printable UTF-8; `score`, 0 to 2^32 − 1, what a game server keeps.

The key of a result is checked by the `ServersService` the servers module provides, which tells the server of a key; the controller reuses the reading of a request the servers module writes (`RouteAnswers.hpp`, `src/master/Servers/Controllers/`).

## The client of the master: `MasterClient`

`MasterClient` (`rtype_network_master_client`, `src/network/Master/Client/`) is what a game server and, to read the games of a server, the game client call: one function per route, each sending the message of its route with the key of `MasterAccess` as Bearer, when it has one.

```cpp
rtype::network::MasterClient master{rtype::network::MasterAccess{
    .url = "https://master.example.org", .key = masterKey}};
master.registerServer(registration);
master.sendHeartbeat(heartbeat);
master.sendResult(gameId, finishedGame);  // PUT /api/servers/me/results/{gameId}

rtype::network::ServerListing servers = master.listServers();  // no key needed
rtype::network::GameListing listing = master.listGames(serverId);
```

`listServers()` reads every page of `GET /api/servers`, `MAXIMUM_PAGE_SIZE` (100) servers each, up to `MAXIMUM_LISTED_SERVER_PAGES` (10) pages, and returns them in one listing.

- **Every call waits for its answer**, `MASTER_CONNECTION_TIMEOUT` (5 s) to connect, then `MASTER_TRANSFER_TIMEOUT` (5 s) for each read and write, so it is made away from the game loop. Two threads never call the same client together.
- **An error status** throws `MasterRefusedException`: `status()` and the `detail` of the problem document. **No answer** throws `MasterUnreachableException`.
- **HTTP or HTTPS:** cpp-httplib is built with OpenSSL, so the url is `http://` or `https://`; any other scheme, or a port outside 1 to 65535, throws `InvalidMasterUrlException`. Over HTTPS, the certificate of the master must be signed, for its host, by a root the system trusts (the Keychain on macOS, the store of Windows, `/etc/ssl` on Linux); an `http://` url sends the key in clear.

## Adding a route

A module listed after `http` in `masterModules()` requires the `MasterHttpServer` while it installs, and adds its route there, before the server opens. Its `dependencies()` names `http`, so the application refuses a list where it comes first. The double of the tests, `RouteAddingModule` (`tests/doubles/`), does it this way:

```cpp
void RouteAddingModule::install(rtype::engine::InstallContext& context) {
  context.require<rtype::master::MasterHttpServer>().add(
      rtype::network::MasterRoute{.method = rtype::network::RouteMethod::Get,
                                  .path = path_},
      []([[maybe_unused]] const rtype::network::ApiRequest& request) {
        return rtype::network::ApiResponse{.status = rtype::master::OK_STATUS,
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
| Change when a server is offline, unreachable or degraded | `OFFLINE_SILENCE`, `PROBE_ANSWER_SILENCE`, `DEGRADED_TICK_P95_MILLISECONDS` and `DEGRADED_LOSS_PERCENT` in `src/master/Servers/Services/Helpers/ServerStatusRuleConstants.hpp`, and `statusOf()` next to them |
| Change how long a token of a probe lives, or how often a destination may be probed | `PROBE_TOKEN_WINDOWS` in `src/master/Servers/Services/ServersServiceConstants.hpp`; `PROBE_MINIMUM_GAP` in `src/master/Probe/ProbeConstants.hpp`, which follows `MINIMUM_NOTICE_GAP` in `src/network/Master/Validators/MasterMessageConstants.hpp` |
| Change how often the statuses are refreshed | `STATUS_REFRESH_INTERVAL` in `src/master/Servers/Jobs/ServerStatusJobConstants.hpp` |
| Change what a result holds, or how the leaderboard ranks | `finishedGameFromText()` and `MasterMessageConstants.hpp` in `src/network/Master/Validators/`; `LEADERBOARD_QUERY` in `src/master/Leaderboard/Repositories/LeaderboardRepositoryConstants.hpp` |
| Change how often a game server beats | `MASTER_HEARTBEAT_INTERVAL` in `src/network/Master/Validators/MasterMessageConstants.hpp`, which the master also reads to tell a silent server |
| Change the size of a page of a list | `MINIMUM_PAGE_SIZE`, `DEFAULT_PAGE_SIZE` and `MAXIMUM_PAGE_SIZE` in `src/network/Master/Routes/MasterRouteConstants.hpp` |
| Call the master from the game server | `MasterClient` (`src/network/Master/Client/`) |
| Change where or how the master is deployed | [Deploying the master](/R-Type/deploy-master/#where-to-intervene) |
| Add a page to the web site | `web/src/app/[lang]/` for its path, its title and its language, then a block in `web/src/components/blocks/` that builds it; its calls to the API in `web/src/lib/api/`, its words in `web/src/i18n/dictionaries/` |
| Change a color, a face or a size of the site | the tokens of `web/src/styles/globals.css` |
| Add or change a word of the site | its label type in the component, then both `web/src/i18n/dictionaries/` |
| Change what the games of a server are listed as | `listedStateOf()` in `ServersService.cpp` (`src/master/Servers/Services/`) and `GameListing` (`src/network/Master/Messages/`) |
| Change what a game server tells of its games | `supervisedHostedGames()` and `supervisedMeasures()` (`src/server/SupervisedGames/SupervisedGamesReport.cpp`) |
| Add a setting of the master | the `install()` of the module that reads it, `master.cfg.example` and the table above |
| Change what an error answers | `problemDetailsText()` in `src/network/Http/Server/HttpServer.cpp`, shared with the supervisor |
| Add a module | `src/master/<Name>/`, its line in `masterModules()` and its row in [the list of modules](/R-Type/project-layout/#the-list-of-modules) |
