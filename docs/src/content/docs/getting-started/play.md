---
title: Play a game
description: Start a server, join it with one to four clients, find a game on the server map, and the controls.
sidebar:
  label: Play a game
  order: 3
---

A game needs a server, which owns the game, and one client per player, which draws it and sends the keys held. The commands below use the binaries of [Build from source](/R-Type/getting-started/build/); an installed client starts from its icon and takes the same options.

## Your first game

In one terminal:

```bash
./r-type_server
```

In another:

```bash
./r-type_client -h 127.0.0.1
```

The server listens on UDP port 4242. Given `-h`, the client joins that server as soon as you press PLAY on the title. In the waiting room, press READY (R): the game starts once every connected pilot is ready.

:::tip[Controls]
The arrows move the ship and the space bar shoots. Escape opens the options during a game; SETTINGS > CONTROLS changes every key, and the choice is kept for the next launch.
:::

## A game of four players

On the machine that hosts, here at `192.168.1.20` on the local network:

```bash
./r-type_server -p 4242
```

On each player's machine, with a different nickname:

```bash
./r-type_client -h 192.168.1.20 -p 4242 -n ACE
./r-type_client -h 192.168.1.20 -p 4242 -n BLAZE
./r-type_client -h 192.168.1.20 -p 4242 -n COMET
./r-type_client -h 192.168.1.20 -p 4242 -n DASH
```

The server sends the four pilots to the same game, where each gets a number from 1 to 4. A fifth player lands in a second game; one server runs up to eight. When the game ends, the end screen ranks the pilots.

:::caution[Behind a firewall or a router]
Open the UDP port of the server to the players, and the TCP port of the same number: a client whose entity sheets differ from the server's, on a modded server for instance, downloads the content pack of the server over HTTP when it joins.
:::

## Finding a server on the map

Without `-h`, PLAY opens the server map: the servers the master lists, placed around your planet by the ping the client measures. Enter or a click joins one; Q (QUICK PLAY) joins a game of the server with the lowest ping that still has a free seat. The map needs the master, `https://master.rtype.app` unless `set master-url` names another: see [Master](/R-Type/master/) to run one, or [Deploying the master](/R-Type/deploy-master/) to host it.

:::note[No master?]
The map then shows `MASTER UNREACHABLE`. Press D (ADDRESS) to open the connection screen, type the address and the port of a server, and press CONNECT.
:::

## Useful options

| Program | Option | Effect |
|---|---|---|
| both | `-p <port>` | the port of the server, 4242 by default |
| client | `-h <address>` | join this server when PLAY is pressed |
| client | `-n <nickname>` | the nickname of the player, `PILOT` by default |
| client | `--game new`, `--game <n>` | open a game of your own, or join game `n` |
| client | `--no-sound` | start without sound |
| server | `--seed <n>` | the same waves in every game |
| server | `--boss` | start every game at the boss, to test the fight |
| both | `--network bad` | play through a simulated bad network |
| both | `--log-level debug` | a more detailed journal |

`--help` prints every option of either program. Lasting settings go in `server.cfg` and `client.cfg`, described in [Configuration commands](/R-Type/commands/); hosting several games is told step by step in [Tutorial: run a server with several games](/R-Type/tutorial-run-several-games/).
