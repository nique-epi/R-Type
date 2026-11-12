---
title: Game servers for a LAN
description: How the game machine of a LAN is created and destroyed, what runs on it, and how to prepare it once.
---

During a LAN the game servers run on a machine of their own on AWS, which exists only while the LAN is open. One workflow creates it and another destroys it; the machine starts every game server of its pool when it boots. Nothing here changes the server itself (see [Server](/R-Type/server/)): it is scripts, systemd units and Terraform, in `infra/`. The master it relies on is told in [Deploying the master](/R-Type/deploy-master/).

This page tells you how that works, how to prepare it once and where to change it. It describes what exists today and says plainly what has not been run yet.

| Part | State | Where |
|---|---|---|
| The workflows that open and close a LAN | Available; `lan-close` not run yet | `.github/workflows/lan-open.yaml`, `lan-close.yaml` |
| The game machine, in a Terraform folder and a state of its own | Available | `infra/terraform-game/` |
| Its network rules, its role and the role of the two workflows | Available | `infra/terraform/game.tf` |
| The script that installs the pool of game servers on a machine | Available; starting the whole pool was not run on a machine yet | `infra/game-servers/install.sh` |
| The script that creates the keys of the master of the pool | Available | `infra/game-servers/create-keys.sh` |
| The script that prepares everything below once | Available | `infra/game-servers/prepare.sh` |
| Starting a server only when the others fill up, or a second machine | Not yet written | |
| Moving the players of a server that stops | Not yet written | |

## The two machines

- **The master machine** always runs: the master, Caddy in front of it, and, between two LANs, one **test game server** the team plays on.
- **The game machine** exists from `lan-open` to `lan-close`: an instance whose CPU is never slowed down (`instance_type` in `infra/terraform-game/variables.tf`, `c7i.xlarge`: 4 vCPU, 8 GiB), with a pool of 6 game servers on ports 4242 to 4247. A game server runs at most 8 games of 4 players, so the pool holds 48 games and 192 players.

The size is **provisional**. One game with four load-test bots took 4.3 % of one core and 13 MiB on an Apple M5 (the supervisor, 0.5 % and 10 MiB), which puts a full pool near 2 cores; this was not measured on AWS, nor with several games at once. Read `top` on the game machine during a test before a LAN.

## Opening and closing a LAN

Both workflows are run by hand (Actions, Run workflow) and wait for the reviewer of the `production` environment, as `deploy-master` does. Only one of them runs at a time.

| | `lan-open` | `lan-close` |
|---|---|---|
| 1 | stops the test game server of the master machine | fails when games run on the game machine, unless `force` is ticked |
| 2 | creates the game machine (`terraform apply` in `infra/terraform-game/`) | destroys the game machine (`terraform destroy`) |
| 3 | waits, up to 15 minutes, until every server of the pool is `online` on the master | starts the test game server of the master machine again |

The game machine is billed from `lan-open` until `lan-close`. When `lan-open` fails after it created the machine, the machine still exists: run `lan-close`.

`lan-open` run while a LAN is open leaves the machine as it is, with the release and the scripts it started on: the script of its first boot runs only once. Run `lan-close`, then `lan-open`, to get a machine on the latest release and the scripts of `main`; it ends the games in progress.

A release published while a LAN is open still deploys the master. The game machine installs a release only when it starts, so it keeps the one it has until `lan-close`.

## What the game machine does when it starts

`infra/terraform-game/user-data.sh` runs once, at the first boot:

1. It narrows the ports the system gives to 40000 to 40999 (`net.ipv4.ip_local_port_range`): a game listens on a port the system picks, and the network rules open that range only.
2. It reads its own public address from the metadata service of AWS, installs the AWS CLI and reads the key of the master of each port in Parameter Store.
3. It writes one `server.cfg` per server in `/var/lib/rtype-servers/<port>/`, which `install.sh` then makes readable by the user `rtype` only, with `master-url`, `master-key`, `public-address` and `server-name` (`LAN 1` to `LAN 6`).
4. It downloads the scripts of `infra/` at the commit the workflow ran on, and runs `install.sh` with the version of the latest release and the ports of the pool.

The ports of the pool and the range of the games are written once, in `infra/terraform/`, which opens them and writes them as tags of the security group `rtype-game`; `infra/terraform-game/` reads the tags, so the machine never uses a port the rules do not open.

`install.sh` stops before changing anything when a server of the pool has no `server.cfg` with a key. It gives each server its own copy of the release, in `/var/lib/rtype-servers/<port>/release/`: a supervisor writes the commands and the journals of its games next to its executable, under names two supervisors would share. It creates the user `rtype`, then writes the service `rtype-server@<port>`, which restarts a server that stopped by itself after 5 seconds, and starts every server of the pool. The lines of a server go to `journalctl -u rtype-server@<port>`.

The script does not depend on AWS: on any Linux machine with systemd, write one `server.cfg` per port in `/var/lib/rtype-servers/<port>/`, then run `VERSION=<x.y.z> PORTS="4242 4243" infra/game-servers/install.sh` as root.

## The pool: every server from the boot

The machine starts the 6 servers of its pool when it boots, and they run until `lan-close`. A player picks one on the map, where each shows its pilots and its games, and Quick Play sends them to the nearest one with a free seat. The servers share the address of the machine and differ by their port; a server sends a player who joins a game to the port of that game, at the same address.

Starting a server only when the others fill up was written, then removed: on one machine it saves nothing, since the machine is billed the same with one server or six, and it made the pool depend on the master during a LAN. It becomes worth it again the day a full pool adds a second machine.

## Preparing it once

A member of the team does this once, after the master is set up (see [Deploying the master](/R-Type/deploy-master/#setting-it-up-once)). `infra/game-servers/prepare.sh` runs steps 1 to 3 below from a computer whose AWS CLI and GitHub CLI are logged in. It reads the region, the master and its instance from the variables of the repository, finds the bucket that holds the state of the master (`--bucket <bucket>` names it), and plans with the instance type the master runs on today. It applies nothing when the plan changes or destroys a resource, and asks before it applies (`--yes` skips the question). Run again, it skips what is done, so it is also how the sixth key is created an hour later. The steps, one by one:

1. Apply `infra/terraform/` again, with `state_bucket` set to the bucket of the Terraform state: it creates the network rules and the role of the game machine, and the role `rtype-lan` of the two workflows. `infra/bootstrap.sh` does it and sets the two variables of step 2.
2. Set the variables `AWS_LAN_ROLE_ARN` (the `lan_role_arn` output) and `TERRAFORM_STATE_BUCKET` in the repository.
3. Create the keys of the pool: `infra/game-servers/create-keys.sh <url of the master> 4242 4243 4244 4245 4246 4247`. It asks the master for one server per port and keeps each key encrypted in Parameter Store under `/rtype/game/<port>/master-key`, without printing it. The master creates 5 servers an hour for one address: the script stops at the sixth and names the ports left; run it again an hour later.
4. Deploy a release to the master with `deploy-master`, so the service of the test game server exists, and write the `server.cfg` of the test server once in `/var/lib/rtype/test-server/` on the master machine, with its own key.
5. Open a LAN once before the real one, join a game on one of its servers, then close it.

## Limits

- **A stop ends the games.** The server has no way to refuse new players and wait for its games to end, so `lan-close` and a new release installed by hand end the games in progress.
- **A server keeps its own copy of the release**, since it writes next to its executable: 6 copies on the machine, and no way back to the previous release as the master has.
- **The pool is bounded by its keys.** One key per server, created in advance; a seventh server needs a seventh key, its port in `last_game_port` of `infra/terraform/variables.tf`, applied again, and a larger machine past 6 servers.
- **A workflow of GitHub creates and destroys a machine.** The role `rtype-lan` reads and writes the state of the game machine only, creates and destroys only instances tagged `part = game`, and is trusted only from the approved environment. Its permissions were validated by `terraform validate`, not by a run yet.
- **The pool does not follow the load.** Every server of the pool runs from the boot to `lan-close`, full or empty; nothing adds a server or a machine when they fill up. The machine costs the same with one server or six, so nothing is saved by starting them one by one.
- **The list of the servers depends on the master.** While the master does not answer, the servers and their games go on, and a player still joins one by its address.
- **The address of the game machine changes at every LAN.** Each server sends it to the master when it registers, so nothing else holds it.

## Where to intervene

| You want to… | Look at |
|---|---|
| Change the size of the game machine | `instance_type` in `infra/terraform-game/variables.tf` |
| Change the size of the pool | `first_game_port` and `last_game_port` in `infra/terraform/variables.tf`; then one more key |
| Change which servers start with the machine | `start_pool()` in `infra/game-servers/install.sh` |
| Change the service of a game server | `install_service()` in `infra/game-servers/install.sh` for its command; `write_server_unit()` in `infra/release.sh` for what the master and every game server share |
| Change what the game machine prepares at its first boot | `infra/terraform-game/user-data.sh` |
| Open a port of the game machine | the rules of `infra/terraform/game.tf`; the range of the games in its `locals` |
| Change what both workflows set up before Terraform | `.github/actions/game-machine-terraform/action.yaml` |
| Change what the two workflows may do on AWS | `aws_iam_policy_document.lan` in `infra/terraform/game.tf` |
| Change the test game server of the master machine | `install_test_server()` in `infra/master/deploy.sh` |
