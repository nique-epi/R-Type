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
| The web site, a Next.js server of its own that calls the API | Available: the pages of the design in French and English (home, download, sector map, a server, hosting, sign-up, leaderboard, wiki, team); the hosting page creates a server with `POST /api/servers`, the sign-up page an account with `POST /api/users`, the sign-in page a session with `POST /api/sessions`, the page `/link` connects a game to an account, the leaderboard reads `GET /api/leaderboard` | `web/` |
| The results of the games and the global leaderboard, `PUT /api/servers/me/results/{gameId}` and `GET /api/leaderboard` | Available; a game server sends its vanilla games after its heartbeats | `src/master/Leaderboard/`, `src/network/Master/` |
| The master on AWS, installed from each release | Available; see [Deploying the master](/R-Type/deploy-master/) | `infra/`, `.github/workflows/deploy-master.yaml` |
| A machine of game servers created for a LAN | Available; see [Game servers for a LAN](/R-Type/deploy-game-servers/) | `infra/game-servers/`, `infra/terraform-game/` |
| Accounts and sessions, over HTTPS only, `POST /api/users` and `POST /api/sessions` | Available; optional module `accounts`, without whose key `RT_PASSWORD_KEY` the master does not start; the web site signs in, and the game client signs in through it with a device link, or with a token given at launch (`--session`) | `src/master/Accounts/`, `src/network/Master/`, `src/network/Hashing/` |
| Wallets of the accounts: kopecks, chips and owned looks, the cashier and the purchase of a look, over HTTPS only, `/api/users/me/wallet` and `/api/users/me/looks` | Available; optional module `wallet`; the games of a signed-in player credit it, and the game client reads and spends it | `src/master/Wallet/`, `src/network/Master/Economy/` |
| The casino: roulette rooms whose rounds follow the clock of the master, `/api/casino/rooms` | Available; optional module `casino`; the game client plays it | `src/master/Casino/`, `src/network/Master/Casino/` |
| Administration | Not yet written | |

## Starting the master

`--mode master` comes first, before every other option; `r-type_server` alone, or `--mode supervisor`, runs the game server, its supervisor, and `--mode game` is the game process a supervisor starts (see [Server](/R-Type/server/#the-three-roles-supervisor-game-and-master)). `chooseServerMode()` (`src/server/LaunchOptions/`) reads the mode, then `MasterLaunchOptions` (`src/master/LaunchOptions/`) reads the rest in one pass, the logging options included (see [Logging](/R-Type/logging/#choosing-the-level-and-the-output-at-launch)).

```bash
RT_PASSWORD_KEY=$(cat ~/.rtype-password-key) ./r-type_server --mode master -p 8081
```

The master stops at its start without the key of the passwords of its accounts, `RT_PASSWORD_KEY`: draw it once with `openssl rand -hex 32` and keep it, since another key makes every stored password unreadable. A master that needs no accounts, such as one for a LAN, runs without a key once its `master.cfg` holds `module casino off`, `module wallet off` and `module accounts off`, in that order, since the casino needs the wallet and the wallet the accounts (see [Accounts and sessions](#accounts-and-sessions-accountsmodule)).

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
| `port` | 0 to 65535 | 8080 | The TCP port the HTTP API listens on, on every interface; 0 lets the system choose a free one, which the journal names. |
| `database` | a path | empty | The SQLite file of the master; empty for `r-type_master.db` next to the executable. |
| `site-key` | a text | empty | The secret the web site names itself by when it creates a server or an account, or signs in, for a visitor (see [Creating a game server](#creating-a-game-server-post-apiservers)); empty, the master reads nothing the site says of its visitor. The `http` module declares it. Set it in `master.cfg`, never on the command line, which every user of the machine can read. |
| `site-url` | an address starting with `https://` or `http://` | `https://rtype.app` | The web site whose page `/link` validates the device links a game signs in through (module `accounts`, see [Device links](#device-links-signing-a-game-in-through-the-site)); the master writes it in each link it opens, without the slash at its end. |
| `wallet-starting-kopecks` | 0 to 999999 | 0 | The kopecks a wallet holds when it opens, at the first read of its account (module `wallet`); a wallet already open keeps its balances. |
| `wallet-starting-chips` | 0 to 999999 | 500 | The chips a wallet holds when it opens. |
| `casino-rooms` | 1 to 16 | 3 | The roulette rooms the casino runs (module `casino`). |
| `casino-seats` | 1 to 32 | 8 | The players each room seats. |
| `casino-betting-seconds` | 5 to 600 | 20 | The seconds the bets of a round stay open. |
| `casino-spinning-seconds` | 1 to 60 | 6 | The seconds the wheel spins once the bets close. |
| `casino-result-seconds` | 1 to 60 | 5 | The seconds the result shows before the next round opens its bets. |

`master.cfg` also takes `admin <nickname>`, a command of the module `accounts`, which makes that account an admin when the master starts (see [Accounts and sessions](#accounts-and-sessions-accountsmodule)). The key of the passwords is no setting: it comes from the environment variable `RT_PASSWORD_KEY`, so it is never written in a file of the master or on its command line.

## Modules

`masterModules()` (`src/master/Modules/`) lists the modules of the master, in the order they install and start; they stop in the reverse order. The master runs no stage of a loop (`MASTER_LOOP_STAGES` is empty): the threads of the HTTP server answer every request.

| Module | Class | Needs | Opens when it starts |
|---|---|---|---|
| `logger` | `MasterLoggerModule` | nothing | the journal |
| `storage` | `MasterStorageModule` | `logger` | the database, migrated |
| `probe` | `MasterProbeModule` | `logger` | a UDP socket on a port the system picks, and ProbeWire, the thread that sends the probes on it |
| `http` | `MasterHttpModule` | `logger` | the port, and the thread that listens on it |
| `servers` | `ServersModule` | `logger`, `storage`, `probe`, `http` | `StatusWatch`, the thread of `ServerStatusJob`; its routes run once `http` listens |
| `accounts` | `AccountsModule` | `logger`, `storage`, `http` | nothing; it makes admins of the accounts `admin` lines name, provides `ISessionAccounts`, and its routes run once `http` listens. Optional: `RTYPE_MODULE_ACCOUNTS` |
| `wallet` | `WalletModule` | `logger`, `storage`, `http`, `accounts` | nothing; it provides `IAccountWallet`, and its routes run once `http` listens. Optional: `RTYPE_MODULE_WALLET`, off whenever `RTYPE_MODULE_ACCOUNTS` is |
| `leaderboard` | `LeaderboardModule` | `logger`, `storage`, `http`, `servers`; `wallet` when it is on | nothing; its routes run once `http` listens |
| `casino` | `CasinoModule` | `logger`, `storage`, `http`, `accounts`, `wallet` | CasinoClock, the thread of `CasinoRoundJob`; its routes run once `http` listens. Optional: `RTYPE_MODULE_CASINO`, off whenever `RTYPE_MODULE_WALLET` is |

The leaderboard comes after the wallet, so it finds the `IAccountWallet` it credits the games with (`find()`, see [Crediting the games](#crediting-the-games)); without the module `wallet` it keeps the results and credits nothing. The modules of another module read it through its `Contract/` folder, which is compiled even when the module is off (see [Project layout](/R-Type/project-layout/#the-code-of-a-module-used-elsewhere)): `ISessionAccounts` (`src/master/Accounts/Contract/`), the account a session token names, and `IAccountWallet` (`src/master/Wallet/Contract/`), the chips the casino stakes and pays and the credit of a game.

The storage starts before the HTTP API, so a database the master cannot open stops it before its port opens, and the API stops before the database closes. The probes start before the HTTP API too, so a heartbeat a route accepts always has its probe sent, and they stop after the routes.

## The HTTP API: `MasterHttpServer`

`MasterHttpServer` (`src/master/Http/`) holds a `network::HttpServer` (`src/network/Http/`, see [Network](/R-Type/network/#http-server-httpserver)), which wraps cpp-httplib and which the supervisor also uses to serve its content pack; no file of the master includes cpp-httplib. Its routes sit under `/api`, answer `GET`, `POST`, `PUT` or `DELETE`, and answer `application/json`; each is declared once, with its method and path, in `MasterRouteConstants.hpp` (`src/network/Master/Routes/`), which the master and `MasterClient` both read; they run on the threads of cpp-httplib, at least 8 of them, and must not block for long. A route reads a `network::ApiRequest`: the body, the key of an `Authorization: Bearer <key>` header (RFC 6750, the scheme in any case) and the address of the client. It answers a `network::ApiResponse`: a status, a body, the `detail` of a refusal, one header, such as `Location`, and the media type of the body, `application/json` unless the route sets another.

`GET /api/health` answers 200 with the version of the master, `MASTER_VERSION`, the version of `project()` in the root `CMakeLists.txt`:

```json
{"version":"1.1.0"}
```

- **Errors:** an answer of 400 or more without a body gets a problem document of RFC 9457, `application/problem+json`, whose `title` is the phrase of its status, with the `detail` of the route when it gives one: `{"status":404,"title":"Not Found"}` for a path no route answers.
- **The address of the client** is the one of the TCP connection, except for a request from `127.0.0.1`, the reverse proxy on the machine: its client is the last address of its `X-Forwarded-For` header, the one the proxy saw (`clientAddressOf()`, `src/network/Http/Server/`). A client that reaches the master from elsewhere cannot name another address, and Caddy drops the `X-Forwarded-For` a client sends before it writes its own. The supervisor serves its content pack with the same server, and reads the TCP peer only. A creation of the web site is the one exception, counted under the visitor the site names (see [Creating a game server](#creating-a-game-server-post-apiservers)).
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
| `/servers/{id}` | the games of a server, its figures, the load of its machine (see [The load of a server](#the-load-of-a-server)) and the pilots of its last 100 samples (the master keeps one sample a second, a page of the list holds 100); a server created and not registered yet shows a waiting notice | the list, `GET /api/servers/{id}/samples` |
| `/host` | the form that creates a server, with no account, then its key, shown once with a button that copies it, and the two lines of `server.cfg` that hold it; the home page, the hosting guide of the wiki and the key H of the sector map lead to it | nothing; it writes with `POST /api/servers` |
| `/signup` | the form that creates an account: a nickname, a password and the same password again, then a welcome; the menu of the site leads a guest to it, and the form and its welcome lead to `/signin` | nothing; it writes with `POST /api/users` |
| `/signin` | the form that signs in, a nickname and a password, then the page `?next=` names, a path of the site only, the home page otherwise; the menu and the header of the site lead a guest to it, and the form leads to `/signup` | nothing; it writes with `POST /api/sessions` |
| `/link` | the page a game sends its player to, `?code=KQFB-WZPM`: a visitor with no session is sent to `/signin` first, then sees the code and the time the game asked, and connects or refuses it; without a code, or with one no link holds, a field for the code the game shows | `GET /api/device-links/{code}`; it writes with `PUT /api/device-links/{code}/decision` |
| `/leaderboard` | the leaderboard screen of the game, by server or for everyone; the scope and the filters are in the address (`?scope=server&server=7&difficulty=hard&pilots=3`) and the page reads the ten best players again at each address | the leaderboard, and the list of servers for the first tab |
| `/wiki/{keys,enemies,accessibility,host}` | the controls, the bestiary, the pilot palettes, the guide to host a server | nothing |
| `/team` | the four members and their GitHub | nothing |

- **The ping of a star** is the mean round trip of the players of its server at its last sample (`averagePingMilliseconds`), since the site cannot measure one from the browser. A server with no player has none, and is placed at `UNMEASURED_PING_MILLISECONDS` (75).
- **The statuses of the master are folded to three**: `online` and `degraded` stay, `offline`, `unreachable` and `maintenance` are drawn offline, since none of them can be played on.
- **The key of a server never leaves the page that created it**: the server action (`createServerAction()`, `web/src/components/blocks/host/`) returns it to that page and writes it in no journal, no address and no store, and the page keeps it in the state of one component, so a reload loses it; the page says so before and after the creation. The form and the action both refuse a name the master would refuse, empty or over `SERVER_NAME_MAXIMUM_BYTES` (32 bytes of UTF-8, so an accented letter counts as two), without asking the master.
- **A creation that fails says why and keeps the name typed**: the 429 of the quota, a name the master refuses (400), and a master that does not answer within 5 seconds or answers anything else each have a message of their own.
- **The site names the visitor it creates a server for**: when `MASTER_SITE_KEY` is set, the request carries it in `X-Site-Key`, and in `X-Site-Client-Address` the last address of the `X-Forwarded-For` the host of the site wrote, which Vercel overwrites with the public address of the client. The master counts the creation under that visitor when the key matches its `site-key` setting (see [Creating a game server](#creating-a-game-server-post-apiservers)). The site must run behind a host that overwrites `X-Forwarded-For`, or a visitor chooses the address it is counted under.
- **The lines of `server.cfg` the page gives** are `set master-url`, with `PUBLIC_MASTER_URL` (`https://master.rtype.app` by default, as `server.cfg.example` writes it), and `set master-key` with the key. `MASTER_URL` is never shown: it may be an address only the site reaches.
- **An account is created on the site, never in the game.** The form and the server action check the same zod schema (`credentialsSchema`, `web/src/lib/rtype/sign-up.ts`), the rules of the master: a nickname of 1 to 16 bytes of printable UTF-8, the blanks around it left out, and a password of 8 to 128 bytes kept as typed. A sign-up the master would refuse never reaches it. An account has no e-mail, so the form asks the password twice and says that a forgotten one cannot be recovered. The password goes from the action to the master alone: it is written in no journal, and the form is gone once the account exists. A nickname taken (409), the quota (429), a refusal (400) and a master that does not answer each have a message, and what was typed stays. The site names its visitor with the site key, as for a server.

- **A session of the site is a cookie the site alone reads.** The sign-in action keeps the token of the master in `rtype_session` (`web/src/lib/session/session-cookie.ts`), `HttpOnly`, `Secure`, `SameSite=Lax`, until the expiry the master gave; the server of the site sends it to the master as Bearer, and it never reaches a page or a script. Every page reads the account of that session with `GET /api/users/me` (`currentAccount()`, `web/src/lib/session/current-account.ts`): a guest sees *Sign in* in the header and *Sign in* and *Sign up* in the menu; a visitor signed in sees the nickname and *Sign out* there instead, which a phone finds in the menu. A session the master no longer knows, or a master out of reach, reads as a guest. *Sign out*, like leaving the page `/link` with another account, closes the session on the master, then forgets the cookie.
- **The page `/link` says what it connects.** It writes the code and the time the game asked in the zone of the browser, and tells the player to connect only a game in front of them: the guard RFC 8628 (section 5.4) gives against a code someone else made a player validate.

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
| 5 | `heartbeat_samples` | the load of the machine of the server in each sample: the share of its processors that is busy, its memory in use and all its memory; empty in the samples kept before, and for a server that does not measure them |
| 6 | `users` | each account: its nickname, unique without case, the 128 bytes kept of its password, its role, the address and the date of its creation |
| 6 | `sessions` | each open session: the fingerprint of its token, its user, the address it was opened from, when it opened and when it expires |
| 6 | `sign_in_failures` | each failed sign-in: its address and its second, forgotten once it no longer counts |
| 7 | `wallets` | the kopecks and the chips of each account, 0 to 999999 each, a `CHECK` refusing any other value, and when the wallet opened |
| 7 | `wallet_movements` | the journal of the wallets: a line per change of a balance, its currency, its signed amount, its reason and the reference of the fact that made it; a line is never changed, so the lines of a currency add up to its balance |
| 7 | `owned_looks` | the looks each account owns, and how it came to own each: `starter`, `purchase` or `game` |
| 7 | `credited_results` | the reference of each result of a game already credited to an account |
| 8 | `casino_seats` | the seats taken in each room of the casino, and when each was last renewed |
| 8 | `casino_bets` | the bets of each account on a round of a room: the kind, the first number and the chips of each |
| 8 | `casino_rounds` | the number each round of a room drew, written once, when its bets close |
| 9 | `game_tickets` | each game ticket given and not yet expired: the fingerprint of the ticket, never the ticket, its account, when it was given, when it expires, and when a result used it |
| 10 | `device_links` | each device link a game asked for: its short code, the key, the fingerprint of its secret, never the secret, the address that asked, when it opened, expires and was last read, and once a player decided, its account and its decision; forgotten an hour after it expired |
| 4 | `game_results` | a row per player of each vanilla game a server reported: its server, the id of the game, the nickname, the account, the score, the date, the difficulty and the number of players; the key is the server, the game and the nickname, compared without case |

## The load of a server

A reader of the sheet of a server sees whether its machine runs flat out. The measure is of the whole machine, every program counted, so a machine another program fills reads as full: that is what a player who joins it gets.

- **The server** sends, in the `measures` of each heartbeat, `cpuPercent`, the share of the processors that was busy since the heartbeat before, and `usedMemoryBytes` and `totalMemoryBytes`. A server that cannot read them, an older server, and the first heartbeat for `cpuPercent`, leave them out: they are never sent as 0.
- **The master** keeps them in `heartbeat_samples` and gives them back in `GET /api/servers/{id}/samples`. It checks each alone, and does not compare the memory in use with all of it. They do not change the status of the server.
- **The site** (`web/src/lib/rtype/server-load.ts`) reads the two gauges in the newest sample alone, and the curves in the samples it read. The memory gauge is `usedMemoryBytes` over `totalMemoryBytes`, at most 100 %. A load is written "loaded" from `BUSY_LOAD_PERCENT` (70 %) and "flat out" from `FLAT_OUT_LOAD_PERCENT` (90 %), in words and not by the color alone. A newest sample without the measure reads "not measured", never 0 %.

## Creating a game server: `POST /api/servers`

A host creates its server on the web site, whose hosting page calls `POST /api/servers` with the name players will read (a call can also be made by hand or by `MasterClient::createServer()`). The master answers 201, `Location: /api/servers/<id>`, and the server with its key:

```json
{"id":7,"name":"Paris 1","key":"rtk_<64 hexadecimal digits>"}
```

- **The key is shown this once.** It is `rtk_`, so a key that leaks in a log or a repository is recognized, then `SERVER_KEY_BYTES` (32) drawn by `hydro_random_buf()` of libhydrogen, which seeds itself from the system (`/dev/urandom` on Linux once `/dev/random` is ready, `/dev/urandom` on macOS, `RtlGenRandom` on Windows), in lowercase hexadecimal: 68 characters. It never goes in a URL or in the journal.
- **The database keeps its fingerprint only**, `hydro_hash_hash()` of the key in the context `FINGERPRINT_CONTEXT`, so a copy of the file lets nobody act as a server. A key is 256 random bits, so a fast hash is enough: nobody can try every key, unlike every password.
- **One address creates `MAXIMUM_CREATIONS_PER_ADDRESS` (5) servers per `CREATION_WINDOW` (1 hour) at most**; the next one answers 429. The count and the insert are one transaction, so requests sent at once cannot pass the quota together. The address is the address of the client (see [The HTTP API](#the-http-api-masterhttpserver)), except for a creation of the web site.
- **The web site creates for its visitors, each with a quota of its own.** The site calls the master from its own server, so all its visitors would share the quota of one address. It sends two headers with a creation: `X-Site-Key`, the secret both hold, and `X-Site-Client-Address`, the address of the visitor. When the `site-key` setting is not empty and `X-Site-Key` equals it, the creation is counted, and kept in `servers.created_by_address`, under the address of `X-Site-Client-Address` (`SiteVisitors`, `src/master/Http/`). In every other case the creation is counted under the address of the client, with no error: an empty setting, a missing or wrong key, or an address that is no address. An address is an IPv4 one, or a text of 45 characters at most made of the characters of an IPv6 one with two colons at least; the master does not parse an IPv6 address further. A visitor and a client that reaches the master directly from the same address share one count.
- **Whoever lacks the secret cannot choose its address.** The two keys are hashed and their hashes compared in constant time (`secretsMatch()`, `src/network/Hashing/`), so the time of an answer says nothing of the secret, and the master never writes it in its journal. Whoever holds the secret names any address, so creates without limit: the secret is 32 random bytes or more (`openssl rand -hex 32`), sent over HTTPS only, and changing it is one line of `master.cfg` and one variable of the site.
- **Anyone who reaches the master may create a server**: a creation asks for no account yet, though accounts exist (see [Accounts and sessions](#accounts-and-sessions-accountsmodule)); the web site will ask for a signed-in host once it signs hosts in. Revoking a key, and giving it an expiry, are not written yet.

The host then writes the key in the `server.cfg` of that server, as the setting `master-key`, next to `master-url`; `server.cfg` is never committed, only `server.cfg.example`, with an empty key. The supervisor of the game server registers and sends its heartbeats from then on (see [Server](/R-Type/server/#link-with-the-master-servermastermodule)). One supervisor is one server on the master: it registers its own UDP port, `maximumGames` 8 (`MAXIMUM_RUNNING_GAMES`), and in each heartbeat the games it runs, numbered by place from 1 to 8 and named `GAME 1` to `GAME 8`; a place freed by a game that ended is given to the next game started, so a number names a place, not one game for ever. Its `measures` are the median and the 95th percentile of the ticks of its slowest game and the mean round trip of its players, each game counting as many times as it has players, and the load of its machine: `cpuPercent`, `usedMemoryBytes` and `totalMemoryBytes` (see [Server](/R-Type/server/#the-load-of-the-machine)); it sends no traffic and no loss.

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
| `measures` | an object; each of `tickP50Milliseconds`, `tickP95Milliseconds`, `receivedBytesPerSecond`, `sentBytesPerSecond`, `lossPercent` (0 to 100), `averagePingMilliseconds`, `cpuPercent` (0 to 100, `MAXIMUM_CPU_PERCENT`), `usedMemoryBytes` and `totalMemoryBytes` is a number from 0, or left out when the server does not measure it, never sent as 0 |

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
             "measures":{"tickP95Milliseconds":4.25,"cpuPercent":62.5,
                         "usedMemoryBytes":3000000000.0,
                         "totalMemoryBytes":8000000000.0}}]}
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
{"playedAt":1800000000,"difficulty":"normal","playerCount":2,"vanilla":true,"victory":true,
 "players":[{"nickname":"ACE","score":12000,"wonLooks":[]},{"nickname":"R9","score":800,"wonLooks":["crown"],"ticket":"rtt_202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f"}]}
```

The answer to `GET /api/leaderboard?difficulty=normal&playerCount=2&limit=1&player=R9`, when this game is the only one of that leaderboard:

```json
{"scores":[{"rank":1,"nickname":"ACE","score":12000,"playedAt":1800000000,"serverId":7,"serverName":"Paris 1"}],
 "player":{"rank":2,"nickname":"R9","score":800,"playedAt":1800000000,"serverId":7,"serverName":"Paris 1"}}
```

- **The route is a `PUT` on the id the server drew**, since the server, not the master, names the game. A game is kept whole and once per server: a result sent again changes nothing, even with other players or scores, and answers 204 as the first time, so a server may send again whatever it is not sure arrived. A second send never replaces or completes the first.
- **The master keeps vanilla games only**: a body with `"vanilla":false` answers 400, and a game that is not vanilla stays in the leaderboard of its server. The master cannot check a score: it trusts the server an admin gave a key to.
- **A row keeps its server**, so `server=<id>` reads one server, each row of the answer names the server of its game, and the rows of a server can be erased with it.
- **One player, one row**: a row a server wrote with an `accountId` is one player under any nickname, its id compared as written (a game server of this version writes none, see below); a guest is one player per nickname, whatever the server. Nicknames are compared as a game server compares them (`nicknameKey()`, SQLite's `NOCASE`): ASCII letters without case, every other character as it is, so `ACE` and `ace` are one guest, `ÉLODIE` and `élodie` two. Among the rows of a player, its best score counts, the earliest game first when two are equal. Equal scores share a rank, and the next rank skips them (1, 1, 3).
- **`player`** names a nickname, compared the same way; the row of the player who played under it in any game of that leaderboard comes in `player` wherever it ranks, under the nickname of its best score, and `player` is left out when it names no one with a score there. A page is `limit` and `offset`, as for [the list of the servers](#listing-the-servers-and-their-history).
- **Bounds of a result**: `playedAt` in seconds since 1970 UTC, from 0 to 2^63 − 1; `difficulty` and each `nickname`, nicknames of the protocol; `playerCount`, the players when the game started, 1 to `MAXIMUM_PLAYERS` (4); 1 to 4 `players`, no more than `playerCount`, since a game server lets nobody in once a game runs, and no nickname twice; `ticket`, left out for a guest, else a game ticket as the master writes it (`rtt_` and 64 lowercase hexadecimal digits, `isWellFormedGameTicket()`), read as none when missing; `accountId`, left out, else 1 to `MAXIMUM_ACCOUNT_ID_SIZE` (64) bytes of printable UTF-8; `score`, 0 to 2^32 − 1, what a game server keeps; `victory`, a boolean, read as `false` when missing; `wonLooks`, the look items the player won, names of `SHIP_LOOK_ITEM_NAMES`, at most `MAXIMUM_WON_LOOKS` (30, the whole catalog, since a game server names each item once), read as none when missing, so a game server older than these two members still reports. A game server writes the `ticket` its player joined with, and leaves `accountId` out: a server cannot know an account, only the ticket a player hands it. The master still reads an `accountId` a server writes, for the ranking above, and never pays it.

The key of a result is checked by the `ServersService` the servers module provides, which tells the server of a key; the controller reuses the reading of a request the servers module writes (`RouteAnswers.hpp`, `src/master/Servers/Controllers/`).

### Crediting the games

When the module `wallet` is on, `LeaderboardService::credit()` pays the players of a result the leaderboard keeps for the first time; a result sent again is not new, so it pays nothing. Each player who names a `ticket` pays, through `IAccountWallet::creditGame()`, the account the master gave that ticket to (see [Game tickets](#game-tickets)), when the ticket was never used and has not expired when the result arrives:

- **Kopecks**: one per `SCORE_POINTS_PER_KOPECK` (10) points of its score, a part of a kopeck lost, and `VICTORY_BONUS_PERCENT` (25 %) of that more when `victory` is true (`kopecksFor()` and `withVictoryBonus()`, `rtype_network_master_economy`, `src/network/Master/Economy/`, which the client also uses to show the gain on its end screen). The balance stops at `MAXIMUM_KOPECKS` (999999).
- **Looks**: each name of `wonLooks`, owned from then on with the origin `game`; a name of no look is skipped, and a look already owned stays as it was.

A credit is one transaction: it marks the ticket used (`used_at`), then writes the kopecks, the looks and the reference `game:<server>:<game>:<nickname>` in `credited_results`, so neither the ticket nor the result pays twice. A guest, a ticket the master never gave, one a result already used and one that expired get nothing, and the `accountId` of a player is never read for a credit. The journal writes `game:<server>:<game>:<nickname> credited through its ticket: <n> kopecks, <m> looks`, or that it paid nothing and why, never the ticket; a credit that fails is written at the `error` level and is not tried again, since the result it pays is kept. A ticket names only the account that asked for it, so no client can name another account; a dishonest host can still invent the score of a ticket it was handed, once, the same trust as the leaderboard.

## Accounts and sessions: `AccountsModule`

A player creates an account named by its nickname, then opens sessions with its password; the session token names the account to the master. The account holds a wallet (see [Wallets](#wallets-walletmodule)) and plays at the casino (see [The casino](#the-casino-casinomodule)); playing a game never asks for one, and a guest earns nothing. The web site signs a visitor in and keeps the token in a cookie of its own (see [The pages of the site](#the-pages-of-the-site)); the game client signs in through the site with a device link (see [Device links](#device-links-signing-a-game-in-through-the-site)), or with the token `--session` gives it (see [Client](/R-Type/client/#account-clientaccountmodule-iaccountsession-iwallet)). The module is optional (`RTYPE_MODULE_ACCOUNTS`), and every route of it answers over HTTPS only.

The other modules read an account through `ISessionAccounts` (`rtype_master_accounts_contract`, `src/master/Accounts/Contract/`), which `AccountsService` implements: `userOfToken(token, now)` gives the account of a session, or nothing for a token that is malformed, unknown or expired, and extends the session as `GET /api/users/me` does. A controller answers a route of an account through `answerAsAccountOf()` (`RouteAnswers.hpp`), which answers 401, with `WWW-Authenticate: Bearer`, when the request carries no token or no session has it, before the body is read.

| Route | Does | Answers |
|---|---|---|
| `POST /api/users` | creates an account from `{"nickname":"ACE","password":"…"}` | 201, `Location: /api/users/<id>`, and `{"id":1,"nickname":"ACE","role":"player"}`; 409 for a nickname taken, 429 over the quota of creations |
| `POST /api/sessions` | opens a session for the same body | 201, `Location: /api/sessions/me`, and `{"token":"rts_<64 hexadecimal digits>","expiresAt":1800604800,"account":{…}}`; 401 for a wrong nickname or password, 429 after too many failures |
| `GET /api/users/me` | reads the account of the token sent as `Authorization: Bearer <token>` | 200 and the account; 401 without a session |
| `DELETE /api/sessions/me` | closes the session of the token | 204, for a session already closed too; 401 without a token |

A body the master cannot read answers 400 naming the member. The token is sent as a Bearer header, never as a cookie: the callers are the game client and the server of the web site, so there is no CORS and no form to forge.

- **Over HTTPS only.** Each route is `MasterRoute::httpsOnly`, and `MasterHttpServer` answers 403 to a request that did not come over HTTPS (`ApiRequest::overHttps`, see [Network](/R-Type/network/#http-server-httpserver)): the master speaks no TLS, so only a request Caddy forwards from the machine counts. A master on a LAN, without Caddy, answers 403 to every route of the accounts.
- **The key of the passwords** is read from the environment variable `RT_PASSWORD_KEY` when the master starts (`PASSWORD_KEY_VARIABLE`): 64 hexadecimal digits, as `openssl rand -hex 32` draws them. It is never in the database, the repository or the journal. Unset or empty, the master stops at its start, before its port opens (`MissingPasswordKeyException`), and so does any other text that is no key (`InvalidPasswordKeyException`); the error names the variable, never its value. A master that must run without accounts turns them off with `module accounts off`, after `module casino off` and `module wallet off`. Another key makes every stored password unreadable. On AWS, the deployment gives the master its key and never changes it (see [Deploying the master](/R-Type/deploy-master/#giving-the-master-its-key-of-the-passwords)).
- **A nickname** is a nickname of the protocol, 1 to 16 bytes of printable UTF-8, and names one account, compared as a game server compares nicknames: `ACE` and `ace` are one account. **A password** is `MINIMUM_PASSWORD_SIZE` to `MAXIMUM_PASSWORD_SIZE` bytes of printable UTF-8 (8 to 128, the defaults of Better Auth).
- **The database keeps 128 bytes per password**: `hydro_pwhash_create()` of libhydrogen, with a salt of its own and `PASSWORD_HASH_ROUNDS` (200 000, about 60 ms in a release build on an Apple M4 Pro), encrypted with the key (`passwordHashOf()`, `src/network/Hashing/`). A stolen database tests no password without the key. A password never reaches the journal.
- **A session** lives `SESSION_LIFETIME` (7 days). A use more than `SESSION_REFRESH_AGE` (1 day) after its expiry last moved moves it to 7 days after that use, so a player who plays every week never signs in again, and a session read often is written once a day: the defaults of Better Auth. The token is `rts_` and 32 random bytes in hexadecimal, shown once; the database keeps its fingerprint (`sessionFingerprintOf()`), so a copy of the file opens no session. Opening a session removes the expired ones.
- **Quotas, per address.** An address creates `MAXIMUM_USERS_PER_ADDRESS` (5) accounts per `USER_CREATION_WINDOW` (1 hour) at most. That quota and a nickname taken are checked before the password is hashed, then again in the transaction of the insert. After `MAXIMUM_SIGN_IN_FAILURES` (5) failed sign-ins in `SIGN_IN_FAILURE_WINDOW` (15 minutes), every sign-in from that address answers 429 before its password is checked.
- **The web site counts for its visitors.** A creation or a sign-in that carries the site key in `X-Site-Key` is counted, and kept in `users.created_by_address`, `sessions.address` and `sign_in_failures.address`, under the visitor of `X-Site-Client-Address`, as a creation of a server is (`SiteVisitors`, `src/master/Http/`): each visitor of the site has its quotas, and one visitor who fails five sign-ins does not lock the others out. Without the key, or with a wrong one, it is counted under the address it came from.
- **The journal** names each sign-in, `ACE signed in from 203.0.113.7`, and each refusal, `sign-in as ACE from 203.0.113.7 refused: wrong nickname or password`.
- **Roles.** An account is a `player` when it is created. A line `admin <nickname>` of `master.cfg` makes that account an `admin` when the master starts, and the journal says so; a line that names no account yet is written in the journal and applies at the next start after the account exists. Removing the line takes the role from nobody. Nothing reads the role yet: the administration is not written.

The module is laid out as `ServersModule`: `AccountsController` and `DeviceLinksController`, `AccountsService` and `DeviceLinksService` (the quotas, the sessions and the links; `Services/Helpers/`, the token, the key and the secret of a link), one repository per table (`UserRepository` for `users`, `SessionRepository`, `SignInFailureRepository`, `DeviceLinkRepository`), and the plain structs of `Models/`.

### Device links: signing a game in through the site

A game never asks for a password: it signs in through the web site, with the OAuth 2.0 Device Authorization Grant of RFC 8628. The game asks for a link and shows its short code; a player signed in on the site approves it on the page `/link`; the game, which reads its link every few seconds, then trades it for a session. The links live in the module `accounts`, since the trade opens a session and the approval reads one.

| Route | Caller | Answers |
|---|---|---|
| `POST /api/device-links` | the game, no credential | 201, `Location: /api/device-links/me`, and `{"secret":"rtd_…","userCode":"KQFB-WZPM","verificationUri":"<site-url>/link","verificationUriComplete":"<site-url>/link?code=KQFB-WZPM","expiresAt":…,"interval":5}`; 429 over the quota |
| `GET /api/device-links/me` | the game, its secret as `Authorization: Bearer rtd_…` | 200 and `{"userCode","status","createdAt","expiresAt","interval"}`, `status` one of `pending`, `approved`, `denied`, `expired`; 401 for no link of that secret; 429 with `Retry-After: 5` for a read before the interval |
| `DELETE /api/device-links/me` | the game, its secret | 204, for a link already gone too; 401 without a secret |
| `GET /api/device-links/{code}` | the site, the session of the player | 200 and the state of the link; 401 without a session; 404 for no link of that code |
| `PUT /api/device-links/{code}/decision` | the site, the session of the player | `{"decision":"approved"}` or `"denied"`: 200 and the state; the same decision sent again is answered the same; 409 once decided otherwise, or expired undecided; 400 for any other word |
| `POST /api/sessions` | the game, its secret, no body | 201 and the session of the account that approved the link, as for a sign-in; 401 for no link of that secret, 409 while it waits, 403 once refused, 410 once expired |

- **The secret names the link of a game, never a path.** It is `rtd_` and 32 random bytes in hexadecimal, shown once; the database keeps its fingerprint (`deviceLinkFingerprintOf()`). A player names a link by its short code, which only a signed-in account may read or decide on.
- **A short code** is 8 letters of `DEVICE_LINK_USER_CODE_ALPHABET`, the 20 consonants RFC 8628 suggests, `KQFB-WZPM`, drawn without bias; the master reads it in any case, with or without its dash (`deviceLinkUserCodeOf()`). A code another live link holds is drawn again, `MAXIMUM_USER_CODE_DRAWS` (8) times at most.
- **A link lives `DEVICE_LINK_LIFETIME` (10 minutes).** Its game reads it every `DEVICE_LINK_INTERVAL` (5 s); a read within `DEVICE_LINK_SHORTEST_READ_GAP` (4 s) of the last one answers 429, RFC 8628's `slow_down`, and leaves the last read as it was. A refusal stays `denied` past the expiry, so the game says it was refused.
- **One link, one session.** The trade reads the link and removes it in one transaction, so a secret sent twice opens one session; the session then opens as a sign-in does (`AccountsService::openSession()`), counted under the address of the game.
- **A decision is written once**, on a link no one decided and not expired: the update names the decider in the same step, so two players who answer at once leave one decision.
- **Quota.** An address asks for `MAXIMUM_DEVICE_LINKS_PER_ADDRESS` (10) links per `DEVICE_LINK_CREATION_WINDOW` (1 hour) at most. A link is forgotten `DEVICE_LINK_KEPT_AFTER_EXPIRY` (1 hour) after it expired, so the quota still counts it. The reads of codes by a signed-in account are not counted: a guess needs an account, and 20 to the 8th codes over 10 minutes leave it nothing to find.
- **The journal** names each session opened through a link, `ACE signed in through a device link from 203.0.113.7`, never a secret, a short code or a token.

## Wallets: `WalletModule`

The wallet module (`src/master/Wallet/`) keeps what an account holds: kopecks, which games pay and the shop spends, chips, which the casino stakes, and the looks the account owns, which the hangar of the client lets it wear. The client only reads and asks: no route lets a player add money, and kopecks come from games and the cashier, chips from the cashier and the casino. The module is optional (`RTYPE_MODULE_WALLET`), needs the accounts, and every route of it answers over HTTPS only, to the account of the token sent as `Authorization: Bearer <token>`, with 401 without a session.

| Route | Does | Answers |
|---|---|---|
| `GET /api/users/me/wallet` | reads the balances | 200 and `{"kopecks":0,"chips":500}` |
| `GET /api/users/me/wallet/movements` | reads the journal, newest first, a page at a time (`?limit&offset`, as [the list of the servers](#listing-the-servers-and-their-history)) | 200 and `{"movements":[{"id":3,"currency":"chips","amount":-25,"reason":"stake","madeAt":1800000000}]}`; 400 for a page out of its bounds |
| `POST /api/users/me/wallet/exchanges` | the cashier: changes `{"from":"kopecks","amount":500}` into the other currency, one for one | 201, `Location: /api/users/me/wallet/exchanges/<id>`, and the exchange with the balances it left in `wallet`; 422 for an amount of none or past 999999, 409 when the balance it takes from holds less, or the other would pass 999999 |
| `GET /api/users/me/wallet/exchanges/{id}` | reads an exchange of the account | 200 and `{"id":4,"from":"kopecks","amount":500,"madeAt":1800000000}`; 404 for an id that names no exchange of the account |
| `GET /api/users/me/looks` | the looks the account owns, in the order of the catalog | 200 and `{"items":["top_hat","cool_shades","mustache"]}` |
| `PUT /api/users/me/looks/{item}` | buys the look of that name at its price; a look owned costs nothing again | 201 bought, 200 already owned, each with `{"item":"crown","wallet":{…}}`; 404 for a name of no look, 422 for a look the shop does not sell, 409 for too few kopecks |
| `POST /api/users/me/tickets` | gives the account a game ticket (see [Game tickets](#game-tickets)) | 201, `Location: /api/users/me/tickets/<fingerprint>`, and `{"ticket":"rtt_202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f","expiresAt":1800021600}`; 429 past 30 tickets in the last hour |
| `GET /api/users/me/tickets/{fingerprint}` | reads a ticket the account was given | 200 and `{"expiresAt":1800021600,"used":false}`; 404 for a fingerprint of no ticket of the account, or of one expired and forgotten |

- **A wallet opens at its first read or use**, with `wallet-starting-kopecks` (0) and `wallet-starting-chips` (500) and the starter kit, the top hat, the cool shades and the mustache (`STARTER_LOCKER_ITEMS`, origin `starter`). A wallet already open keeps its balances when the settings change.
- **The journal is the truth, the balance its cache.** Each change writes the balance and its line of `wallet_movements` in one transaction, so the lines of a currency add up to its balance. A line gives a `reason`: `opening`, `game`, `exchange`, `purchase`, `stake`, `refund` or `payout`. Its reference names the fact that made it (`exchange:<id>`, `look:<name>`, `game:…`, `round:<room>:<round>`), and a fact that must apply once is looked up before it is written.
- **No balance leaves 0 to 999999** (`MAXIMUM_KOPECKS`, `MAXIMUM_CHIPS`): a spending that would go under 0 is refused, the `CHECK` of the table refuses what the rules would let through, and a credit stops at the largest balance.
- **An exchange** writes two lines naming one exchange, whose id is the number of the line that took the amount. **A purchase** takes the price of the look (`lookItemPrice()`, `ShipLookPriceConstants.hpp`, `src/network/Master/Economy/`) and makes the look owned (origin `purchase`) in one transaction, so a `PUT` sent twice takes the price once.
- **What the other modules move** goes through `IAccountWallet` (`rtype_master_wallet_contract`, `src/master/Wallet/Contract/`), which `WalletService` implements: `applyChips()` stakes, hands back or pays chips inside the transaction its caller opened, and refuses a movement meant to apply once, such as a payout, when a line of the same reason and reference is in the journal; `creditGame()` credits a game in a transaction of its own (see [Crediting the games](#crediting-the-games)).

### Game tickets

A game ticket is how a game pays the right account. A signed-in client asks one (`POST /api/users/me/tickets`) right before it joins a game, and sends it to the game server in its `ConnectionRequest` (see [Protocol](/R-Type/protocol/#version-18)); the server keeps it with the player and reports it with the result, and the master pays the account it gave the ticket to (see [Crediting the games](#crediting-the-games)). The id of an account would name any account its sender likes; a ticket names only the account that asked for it.

- **A ticket is `rtt_` and 32 random bytes** in lowercase hexadecimal, drawn by `fillWithRandomBytes()` as the tokens of the sessions are (`newGameTicket()`, `src/master/Wallet/Services/Helpers/`), and shown once. The master keeps its fingerprint, its keyed hash in its own context (`gameTicketFingerprintOf()`), so a copy of the file presents no ticket; `Location` names the ticket by that fingerprint, never by the ticket.
- **A ticket pays one result**, before `GAME_TICKET_LIFETIME` (6 hours) passed since it was given, counted when the result arrives: the credit marks it used in its own transaction, so two results that present it at once pay one. A player who plays several games in one session joins every one of them with the ticket of that session, so only its first game pays.
- **Quota, per account**: `MAXIMUM_GAME_TICKETS_PER_WINDOW` (30) tickets per `GAME_TICKET_WINDOW` (1 hour), 429 past it. Giving a ticket forgets the tickets expired, so an account holds 180 rows at most.
- **`GET` of the `Location`** answers the account the ticket was given to only: any other account gets 404, as for a fingerprint of no ticket.

The module is laid out as `ServersModule`: `WalletController` and `GameTicketController`, `WalletService` and `GameTicketService` (`Services/Helpers/`, the drawing and the fingerprint of a ticket), `WalletRepository`, `WalletMovementRepository`, `OwnedLookRepository`, `CreditedResultRepository` and `GameTicketRepository`, and the plain structs of `Models/`.

## The casino: `CasinoModule`

The casino module (`src/master/Casino/`) runs the roulette rooms every player of every server shares: fixed rooms, `casino-rooms` (3) of them, each seating `casino-seats` (8) players, named `Bydo Palace`, `Nebula Lounge`, `Wave Cannon Club` and on (`CASINO_ROOM_NAMES`), then `Room <n>`. The master times the rounds, draws the numbers and settles the chips; the client only reads and asks, again every `CASINO_ROOM_POLL_INTERVAL` (500 ms) after the previous answer (see [Client](/R-Type/client/#casino-casinoentrancescreen-casinohallscreen-cashierscreen-roulettescreen)). The module is optional (`RTYPE_MODULE_CASINO`) and needs the accounts and the wallet.

| Route | Does | Answers |
|---|---|---|
| `GET /api/casino/rooms` | lists the rooms, to anyone, over HTTP too | 200 and `{"rooms":[{"id":1,"name":"Bydo Palace","seats":8,"seated":2,"round":58064516,"phase":"betting"}]}` |
| `GET /api/casino/rooms/{room}` | the state of a room, to anyone, over HTTP too (below) | 200; 404 for a room the casino does not run |
| `PUT /api/casino/rooms/{room}/seats/me` | seats the account of the token, or renews its seat | 201 seated, 200 renewed, each with the state of the room; 409 when every seat is taken by others; 404 |
| `DELETE /api/casino/rooms/{room}/seats/me` | gets the account up | 204, for an account already up too |
| `PUT /api/casino/rooms/{room}/rounds/{round}/bets/me` | replaces every bet the account holds on that round by `{"bets":[{"kind":"straight","firstNumber":17,"amount":10}]}`; `{"bets":[]}` takes them all back | 200 and `{"round":…,"bets":[…],"stake":10,"chips":490}`; 409 for closed bets or no seat, 422 for a broken rule or too few chips; 404 |
| `GET /api/casino/rooms/{room}/rounds/{round}/bets/me` | the bets of the account on that round, and what they paid | 200 and `{"round":…,"bets":[…],"stake":10,"payout":360,"winningNumber":17}`, `payout` and `winningNumber` null until the round is drawn; 404 |

The seats and the bets answer over HTTPS only, to the account of a session, with 401 without one; but for `DELETE`, a room that is no number from 1 to `casino-rooms`, or a round that is no whole number from 0, answers 404. The state of a room holds its `id`, `name`, `seats`, `round` and `phase`, the dates of its phases in milliseconds since 1970 (`closesAt`, `resultAt`, `endsAt`) with `serverTime`, the clock of the master when it answered, so a client counts down without trusting its own clock, `winningNumber`, null until the round is drawn, `history`, the last `CASINO_HISTORY_LENGTH` (10) numbers drawn before this round, newest first, and `players`, each seated player with its `nickname`, its `stake` on the round, its `payout`, null until the round is drawn, and its `bets` on the round, in the form `…/bets/me` takes them, so the others seated see where its chips lie. A reader keeps `MAXIMUM_READ_ROULETTE_BETS` (64) bets of a player at most, and reads none when the member is left out.

**The round follows the clock.** A round lasts `casino-betting-seconds` (20 s), then `casino-spinning-seconds` (6 s), then `casino-result-seconds` (5 s): 31 s. Round n starts n cycles after 1970 (`scheduledRoundAt()`, `Services/Helpers/RouletteSchedule.hpp`), so every room plays the same round, a client and the master always agree on the phase, no timer is kept, and a master that restarts falls back on the right round. Its bets are open from its start to `closesAt` excluded, the wheel spins until `resultAt`, and the result shows until `endsAt`, the start of the next round. A round goes on with nobody seated. A change of a duration changes the number of every round.

- **The draw.** `CasinoRoundJob` runs on CasinoClock, its own thread, which the module starts with the master and stops with it: at once, then every `CASINO_ROUND_INTERVAL` (250 ms), it asks `RouletteDealer` for every round due, the round of each room whose bets closed and every earlier round that holds bets and was never drawn, such as one whose bets closed while the master was down. Each round is drawn in one transaction: the number of `IRouletteDraw` (`SystemRouletteDraw`, `src/network/Master/Casino/Draw/`, every number as likely as another) written in `casino_rounds`, and each payout added to the wallets under the reference `round:<room>:<round>`. A round already drawn is never drawn again. The journal writes `round <round> of room <room> drew <number>`; a pass that fails is written there and tried again an interval later.
- **The bets.** `PUT …/bets/me` is one transaction: the round must be the one whose bets are open and must hold no row of `casino_rounds`, the check that keeps a bet off a round the job draws at the same moment, since both go through the lock of the database; the account must be seated. The bets it held are handed back (`refund`), then the new ones checked: on the cloth (`isPlaceableBet()`, `src/network/Master/Casino/Cloth/`) with at least one chip each, `MAXIMUM_ROULETTE_BETS` (32) at most, `MAXIMUM_ROUND_STAKE` (5000) chips at most in all, and no more chips than the wallet holds; then they are staked (`stake`). A refusal writes nothing. A winning bet returns its stake times its return, the stake included (`payoutOf()`): 36 for a straight, 18 a split, 12 a street, 9 a corner, 6 a six line, 3 a dozen or a column, 2 an even chance.
- **The seats.** A seat counts while it was renewed in the last `CASINO_SEAT_LIFETIME` (30 s); the client renews it every `CASINO_SEAT_RENEWAL_INTERVAL` (10 s), so a player who left without getting up frees its place.
- **Tidying.** Once it drew a round, the dealer removes the seats not seen for 30 s, the bets of rounds drawn more than `DRAWN_BETS_KEPT_FOR` (24 hours) ago, and the rounds of each room older than its last 10 that no kept bet names.

The module is laid out as `ServersModule`, with `Jobs/` for `CasinoRoundJob`; `RouletteDealer` sits in `Services/` beside `CasinoService`, and `Services/Helpers/` holds the schedule, the payouts of a round, the names of the rooms and the reference of a round. The rules of the cloth, its constants (`RouletteConstants.hpp`) and the draw are in `src/network/Master/Casino/`, so the client lays the bets the master takes.

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

Two more clients in the same library serve the game client, each taking the token of a session as an argument, sent as Bearer and never kept: `MasterAccountClient`, the account of the token, its wallet, its looks, a purchase and an exchange, and `MasterCasinoClient`, the rooms, their state, a seat and the bets of a round. The routes of an account answer over HTTPS only, so their url names an `https://` master, or one behind a proxy that ends TLS. All three send through `MasterTransport`, which holds the connection and its timeouts.

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
| Answer a route over HTTPS only | `.httpsOnly = true` in its `MasterRoute`; `MasterHttpServer` answers 403 to a request in clear |
| Change the quotas of the accounts, or how long a session lives | `src/master/Accounts/Services/AccountsServiceConstants.hpp` |
| Change what a wallet opens with | the settings `wallet-starting-kopecks` and `wallet-starting-chips`; their defaults in `src/master/Wallet/WalletModuleConstants.hpp`; the starter kit in `src/network/Master/Economy/StarterLookConstants.hpp` |
| Change what a game pays, or the price of a look | `SCORE_POINTS_PER_KOPECK` and `VICTORY_BONUS_PERCENT` in `src/network/Master/Economy/KopecksConstants.hpp`, `ShipLookPriceConstants.hpp` beside it: the master and the client both read them |
| Change the rules of the wallets, the cashier or a purchase | `WalletService` in `src/master/Wallet/Services/` |
| Change the rooms of the casino or the length of a round | the settings `casino-rooms`, `casino-seats` and `casino-*-seconds`; their defaults and bounds in `src/master/Casino/CasinoModuleConstants.hpp`, the names in `Services/Helpers/CasinoHelpersConstants.hpp` |
| Change the rules of the cloth, the limits of a round or the returns of a bet | `src/network/Master/Casino/Cloth/`: `RouletteRules.cpp` and `RouletteConstants.hpp`, shared with the client |
| Change how a round is drawn and paid, or what is tidied | `RouletteDealer` in `src/master/Casino/Services/`, `CASINO_ROUND_INTERVAL` in `src/master/Casino/Jobs/CasinoRoundJobConstants.hpp` |
| Change how long a seat lasts | `CASINO_SEAT_LIFETIME` and `CASINO_SEAT_RENEWAL_INTERVAL` in `src/network/Master/Casino/Table/CasinoRoomConstants.hpp` |
| Read the account of a session in another module | `ISessionAccounts` in `src/master/Accounts/Contract/`, and `answerAsAccountOf()` in `RouteAnswers.hpp` |
| Change the bounds of a password, or the cost of its hash | `MINIMUM_PASSWORD_SIZE` and `MAXIMUM_PASSWORD_SIZE` in `MasterMessageConstants.hpp`; `PASSWORD_HASH_ROUNDS` in `src/network/Hashing/HashingConstants.hpp` |
| Change what a server sends, or its bounds | `src/network/Master/`: `MasterMessageText.cpp` and `MasterMessageConstants.hpp` |
| Change what a creation, a registration or a heartbeat answers | `ServersController` (`src/master/Servers/Controllers/`) |
| Change the quota of creations | `MAXIMUM_CREATIONS_PER_ADDRESS` and `CREATION_WINDOW` in `src/master/Servers/Services/ServersServiceConstants.hpp` |
| Change which creations are counted under a visitor, or what an address is | `visitorAddressOf()` in `src/master/Http/SiteVisitorAddress.cpp`; the headers in `src/master/Http/HttpConstants.hpp` |
| Change how long samples and changes of status are kept | `SAMPLE_RETENTION` in `src/master/Servers/Repositories/ServersRepositoryConstants.hpp`; `STATUS_CHANGE_RETENTION` follows it |
| Change when a server is offline, unreachable or degraded | `OFFLINE_SILENCE`, `PROBE_ANSWER_SILENCE`, `DEGRADED_TICK_P95_MILLISECONDS` and `DEGRADED_LOSS_PERCENT` in `src/master/Servers/Services/Helpers/ServerStatusRuleConstants.hpp`, and `statusOf()` next to them |
| Change how long a token of a probe lives, or how often a destination may be probed | `PROBE_TOKEN_WINDOWS` in `src/master/Servers/Services/ServersServiceConstants.hpp`; `PROBE_MINIMUM_GAP` in `src/master/Probe/ProbeConstants.hpp`, which follows `MINIMUM_NOTICE_GAP` in `src/network/Master/Validators/MasterMessageConstants.hpp` |
| Change how often the statuses are refreshed | `STATUS_REFRESH_INTERVAL` in `src/master/Servers/Jobs/ServerStatusJobConstants.hpp` |
| Change what a result holds, or how the leaderboard ranks | `finishedGameFromText()` and `MasterMessageConstants.hpp` in `src/network/Master/Validators/`; `LEADERBOARD_QUERY` in `src/master/Leaderboard/Repositories/LeaderboardRepositoryConstants.hpp` |
| Change how often a game server beats | `MASTER_HEARTBEAT_INTERVAL` in `src/network/Master/Validators/MasterMessageConstants.hpp`, which the master also reads to tell a silent server |
| Change the size of a page of a list | `MINIMUM_PAGE_SIZE`, `DEFAULT_PAGE_SIZE` and `MAXIMUM_PAGE_SIZE` in `src/network/Master/Routes/MasterRouteConstants.hpp` |
| Call the master from the game server | `MasterClient` (`src/network/Master/Client/`) |
| Change where or how the master is deployed | [Deploying the master](/R-Type/deploy-master/#where-to-intervene) |
| Add a page to the web site | `web/src/app/[lang]/` for its path, its title and its language, then a block in `web/src/components/blocks/` that builds it; its calls to the API in `web/src/lib/api/`, its words in `web/src/i18n/dictionaries/`; a page that writes to the master does it in a server action beside its block, as `create-server.action.ts` does |
| Change a color, a face or a size of the site | the tokens of `web/src/styles/globals.css` |
| Add or change a word of the site | its label type in the component, then both `web/src/i18n/dictionaries/` |
| Change what the games of a server are listed as | `listedStateOf()` in `ServersService.cpp` (`src/master/Servers/Services/`) and `GameListing` (`src/network/Master/Messages/`) |
| Change when the site calls a server loaded or flat out | `BUSY_LOAD_PERCENT` and `FLAT_OUT_LOAD_PERCENT`, `web/src/lib/rtype/server-load.ts` |
| Change what a game server tells of its games | `supervisedHostedGames()` and `supervisedMeasures()` (`src/server/SupervisedGames/SupervisedGamesReport.cpp`) |
| Add a setting of the master | the `install()` of the module that reads it, `master.cfg.example` and the table above |
| Change what an error answers | `problemDetailsText()` in `src/network/Http/Server/HttpServer.cpp`, shared with the supervisor |
| Add a module | `src/master/<Name>/`, its line in `masterModules()` and its row in [the list of modules](/R-Type/project-layout/#the-list-of-modules) |
