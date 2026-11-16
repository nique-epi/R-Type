---
title: Deploying the master
description: How a release reaches the master instance on AWS, how to set it up once, and where to change it.
---

The master (see [Master](/R-Type/master/)) runs on one small AWS machine, described with Terraform in `infra/`. Publishing a release installs its server package on that machine, after a person approves it in GitHub; no key to AWS is stored anywhere. This page tells you how that works, how to set it up once and where to change it. It describes what exists today and says plainly what has not been run yet.

| Part | State | Where |
|---|---|---|
| The machine, its address, its network rules and the role of the deployments, in Terraform | Available; not applied to an account yet | `infra/terraform/` |
| The script that sets everything up once: bucket, Terraform, variables, environment, DNS wait | Available; not run yet | `infra/bootstrap.sh` |
| The script that installs a release and goes back when the master does not answer | Available | `infra/master/deploy.sh`, after the functions of `infra/release.sh` |
| The timer that restarts a master that stopped answering | Available; not deployed yet | `install_health_check()` in `infra/master/deploy.sh` |
| The test game server of the machine, between two LANs | Available; not deployed yet | `install_test_server()` in `infra/master/deploy.sh` |
| The workflow that runs the script, on every release or by hand | Available | `.github/workflows/deploy-master.yaml`, called by `release.yaml` |
| The check of the Terraform files and of the scripts, on a pull request | Available | `.github/workflows/infra.yaml` |
| HTTPS through Caddy, for a domain name | Available; not deployed yet | `infra/master/deploy.sh`, `master_domain` in `variables.tf` |
| The key of the passwords of the accounts, from a secret of GitHub | Available; not deployed yet | `install_password_key()` in `infra/master/deploy.sh`, the secret `MASTER_PASSWORD_KEY` |
| A backup of the database | Not yet written | |

## What runs on AWS

- **One instance**, `t3.small` (an x86 machine with 2 vCPU and 2 GiB of memory; `instance_type` in `variables.tf`), a type the free plan of AWS may launch running Ubuntu 24.04, with an 8 GiB disk. The package of a release is built on Ubuntu 22.04 for x86, and its C library must be at least that of the build, so a machine with an older one, such as Amazon Linux 2023, cannot run it.
- **One address that does not change**, an Elastic IP (`aws_eip.master`). The A record of the domain of the master (`master_domain`) points at it, so Terraform refuses to destroy it (`prevent_destroy`).
- **Two open ports**, from anywhere: 443, the HTTPS API, and 80, which only answers the certificate challenges of Let's Encrypt and sends the rest to HTTPS. The master listens on 8080 (`master_port`), which no rule opens. There is no SSH and no key pair: the machine is reached through AWS Systems Manager (SSM), which needs no open port.
- **Caddy**, the package of Ubuntu, in front of the master: it answers HTTPS for `master_domain` with a certificate of Let's Encrypt it gets at the first deployment and renews by itself, and forwards each request to `127.0.0.1:8080`.
- **One systemd service**, `rtype-master`, which runs `r-type_server --mode master -p 8080` as the user `rtype`, with the database in `/var/lib/rtype/r-type_master.db`. systemd gives it `RT_PASSWORD_KEY`, the key of the passwords of the accounts, from `/etc/rtype/master.env`, a file only root reads (see [Giving the master its key of the passwords](#giving-the-master-its-key-of-the-passwords)); without that file the master runs without accounts, and every route of the accounts answers 503 (see [Master](/R-Type/master/#accounts-and-sessions-accountsmodule)). It writes no journal file; its lines go to `journalctl -u rtype-master`. The port in the unit and `master_port` must stay equal. systemd starts it again 5 seconds after it stopped by itself.
- **A health check**, `rtype-master-health.timer`: every 30 seconds it asks `GET /api/health` on the machine, and restarts the master when two requests 5 seconds apart both fail. It covers a master that still runs and no longer answers, which the restart of the service does not.
- **A test game server**, `rtype-test-server`, the one the team plays on between two LANs. It reads `/var/lib/rtype/test-server/server.cfg`, which a person writes once with its own key of the master, and runs from its own copy of the release. The deployment never enables it: `lan-close` starts it and `lan-open` stops it (see [Game servers for a LAN](/R-Type/deploy-game-servers/)). The network rules of Terraform do not open its ports.
- **A CPU on credits.** A `t3` machine earns CPU credits at a baseline of 20% of each vCPU and spends them when it works harder. The instance is set to `cpu_credits = "standard"`: once the credits are gone, the CPU is slowed down, and the bill never exceeds the price of the instance. The master is an HTTP API on a SQLite file, which stays far below that baseline. A machine whose CPU never slows down (`c7i-flex.large`, `c5.large`) costs more.

The size is **provisional**: measure the free memory with `free -m` and the credits left (`CPUCreditBalance` in CloudWatch) on the running master before keeping it. The cost is an estimate from public prices, **not measured**: about 20 USD a month, of which about 16 for the machine (0.0216 USD an hour in `eu-north-1`).

On the **free plan** of AWS, only the types `aws ec2 describe-instance-types --filters Name=free-tier-eligible,Values=true` lists can be launched, which is why the default is `t3.small` and not `t3.medium`. The cost is taken from the credits of the account, and the plan ends after six months or when the credits are used up: AWS then closes the account, the master stops and its database is deleted 90 days later. Upgrade the account to the paid plan to keep the master past that date; `instance_type` can then go back to `t3.medium`, which replaces the instance and starts the database again.

## The deployment: `deploy-master`

The workflow `deploy-master` runs in the GitHub environment `production`, so it waits for the reviewer that environment names. It runs after the `publish` job of `release.yaml` on every tag `v*`, and by hand (Actions, `deploy-master`, Run workflow, with a tag such as `v0.1.0`) to redeploy or to go back to an older release. Only one deployment runs at a time.

| Step | What happens |
|---|---|
| 1 | The tag is checked: it must read `vX.Y.Z`. |
| 2 | The job assumes the AWS role `rtype-master-deploy` with a short-lived token of GitHub (OpenID Connect). The role trusts only this repository's `production` environment, and may only run a script on this one instance. |
| 3 | The job reads the domain of `MASTER_URL`, which must be `https://<domain>`, with no port and no path. |
| 4 | The job checks the secret `MASTER_PASSWORD_KEY`: empty, or 64 lowercase hexadecimal digits. Its value is never printed. |
| 5 | The job sends `infra/release.sh` and `infra/master/deploy.sh`, preceded by the lines `VERSION=<x.y.z>`, `DOMAIN=<domain>` and `PASSWORD_KEY=<key, or nothing>`, to the instance through SSM Run Command (`infra/run-on-instance.sh`), and waits for it. |
| 6 | The job calls `GET /api/health` on `MASTER_URL` from the outside, for up to a minute, the time Let's Encrypt takes to sign the first certificate. |

`deploy.sh` runs as root on the instance:

1. It checks the key of the passwords it was given against the one the master has: a different key stops the script here, before anything changed (see [Giving the master its key of the passwords](#giving-the-master-its-key-of-the-passwords)).
2. It downloads `R-Type-server-<version>-Linux.tar.gz` from the public release, and unpacks it in `/opt/rtype/releases/`. A release that does not exist stops the script here, before anything changed.
3. It writes the unit, and `/etc/rtype/master.env` when the master has no key yet and the job gave one, points `/opt/rtype/current` at the new release and restarts the service.
4. It waits up to 20 seconds for `GET /api/health` to answer 200. When it does not, it prints the last 30 lines of the journal, points `current` back at the previous release, restarts and fails.
5. It deletes the releases other than the new one and the previous one.
6. It writes the health check and starts its timer, then gives the test game server a copy of the new release and restarts it when it runs, which ends its games.
7. It installs Caddy once, writes `/etc/caddy/Caddyfile` for `DOMAIN`, checks it with `caddy validate` and reloads Caddy.

Running the same tag again installs nothing new and restarts the master. The check only reads the status code: `MASTER_VERSION` is written by hand in `src/master/MasterConstants.hpp`, it does not follow the tag.

Going back after the database was migrated is not guaranteed: the new master migrates the database when it starts, and an older one may not read a newer schema.

## Setting it up once

A member of the team does this once, with their own AWS, GitHub and Vercel accounts; nothing here runs by itself. `infra/bootstrap.sh --domain <domain> --bucket <bucket>` runs steps 1 to 4 below and waits for the A record of step 2 (`--deploy <tag>` also does step 5); it asks for the AWS account and for the Terraform plan first, and running it again skips what is done. It leaves the address of the master in Vercel, the game servers and the client to you, the key of the site (see [Giving the site its key](#giving-the-site-its-key)) and the key of the passwords (see [Giving the master its key of the passwords](#giving-the-master-its-key-of-the-passwords)). The steps are in the [README of `infra/`](https://github.com/nique-epi/R-Type/blob/main/infra/README.md), in this order:

1. Create the S3 bucket of the Terraform state, then `terraform init`, `plan` and `apply` from `infra/terraform/`, with `master_domain` set to the domain of the master and `state_bucket` to that bucket.
2. At the DNS provider of that domain, point an `A` record at the `master_public_ip` output.
3. Create the four variables of the repository the deployment reads, from the outputs of Terraform: `AWS_REGION`, `AWS_DEPLOY_ROLE_ARN`, `MASTER_INSTANCE_ID` and `MASTER_URL`. The workflows of a LAN read two more (see [Game servers for a LAN](/R-Type/deploy-game-servers/#preparing-it-once)). They are variables, not secrets. The only secret is `MASTER_PASSWORD_KEY`, a secret of the environment `production`, which the master needs for its accounts (see [Giving the master its key of the passwords](#giving-the-master-its-key-of-the-passwords)).
4. Create the environment `production` with at least one required reviewer: the deployment waits for it, and the AWS role trusts only that environment.
5. Run `deploy-master` by hand with the tag of an existing release, and check that the instance is `Online` in SSM and that `GET /api/health` answers over HTTPS.
6. Give the `master_url` output to the web site, the game servers and the client.

The README also lists the symptoms of each step that goes wrong.

## Giving the address to the web site

The web site calls the master from its own server, at `MASTER_URL` (see [Master](/R-Type/master/#the-web-site)), and Vercel hosts it. Set `MASTER_URL` to the `master_url` output in the Vercel project, for the Production and Preview environments, then deploy the site again: a variable only reaches the deployments made after it changed, and `next.config.ts` reads it at the build too. The browser never calls the master, so the master needs no CORS.

## Giving the site its key

Every visitor of the site reaches the master from the address of Vercel, so they would share one quota of creations. The site and the master hold one secret, with which the site names the visitor it creates a server for (see [Master](/R-Type/master/#creating-a-game-server-post-apiservers)). Nothing sets it by itself, and without it the site still works, all its visitors under one quota:

1. Draw it once: `openssl rand -hex 32`. Letters and digits only, so its line of `master.cfg` is never refused, and never written in the journal.
2. On the instance (an SSM session), write the line `set site-key <secret>` in `/var/lib/rtype/master.cfg`, the folder the master starts from, readable by the user `rtype` only (`chown rtype:rtype`, `chmod 600`), then `systemctl restart rtype-master`. A deployment does not touch that file.
3. In the Vercel project, set `MASTER_SITE_KEY` to the same value, as a sensitive variable, for Production only, then deploy the site again.

The secret is in neither GitHub nor Terraform. To change it, do the three steps again; between steps 2 and 3 the creations of the site are counted under the address of Vercel.

## Giving the master its key of the passwords

A player who creates an account on the master chooses a password. The master keeps a hash of it in its database, encrypted with one key, `RT_PASSWORD_KEY`, so a copy of the database alone tests no password (see [Master](/R-Type/master/#accounts-and-sessions-accountsmodule)). The master reads that key from its environment when it starts; without it, it runs and refuses every route of the accounts with 503.

The key is kept in two places, and nowhere else: the secret `MASTER_PASSWORD_KEY` of the GitHub environment `production`, which nobody can read back, and `/etc/rtype/master.env` on the instance, owned by root, mode 0600, which systemd reads before it starts the master as the user `rtype`. It is never written in the unit, which every user of the machine reads, nor on the command line, the journal, the repository or Terraform.

Create it once, before the first deployment that should open the accounts; it never goes through your screen or your shell history:

```bash
openssl rand -hex 32 | gh secret set MASTER_PASSWORD_KEY --env production
```

What a deployment does then:

| The secret | `/etc/rtype/master.env` on the instance | The deployment |
|---|---|---|
| empty or missing | missing | runs the master without accounts, and says so in its output |
| set | missing | writes the file, then restarts the master with its accounts |
| set, the same key | present | changes nothing to the key |
| empty or missing | present | keeps the file: the master keeps its key and its accounts |
| set, another key | present | stops before it installs anything, and the master keeps running on its key |

`release.yaml` passes the secret to `deploy-master` by name: a called workflow sees a secret of an environment only when its caller passes it, and reads it as empty otherwise, with no error.

**Changing the key loses every password.** The master cannot encrypt the stored hashes again under a new key yet, so after a change every sign-in of an existing account answers 401, as for a wrong password, while its nickname stays taken; the sessions already open go on until they expire, since their tokens do not depend on the key. Change it only when it leaked:

1. Set the new key: `openssl rand -hex 32 | gh secret set MASTER_PASSWORD_KEY --env production`.
2. On the instance (an SSM session), remove the file of the old key: `rm /etc/rtype/master.env`. Without this step the next deployment stops on "MASTER_PASSWORD_KEY differs from the key the master has".
3. Run `deploy-master` by hand with the tag in use: it writes the new key and restarts the master.

A secret overwritten by mistake does the opposite: the next deployment stops, and the master keeps its key. Set the right key again, or remove the file on purpose.

## Limits

- **HTTPS ends at Caddy.** Between Caddy and the master, on the machine, requests go in plain HTTP to `127.0.0.1:8080`. The master reads the address of each client in the `X-Forwarded-For` header Caddy writes, and trusts it only from `127.0.0.1` (see [Master](/R-Type/master/#the-http-api-masterhttpserver)).
- **The key of the passwords travels through Run Command.** The job writes it in the script it sends, so `aws ssm list-commands` shows it, in the parameters of each deployment, to whoever may call that action in the AWS account; the role of the deployments may not. GitHub hides it in the log of the job.
- **Whoever holds the key of the site creates servers without limit**, since it names the address each creation is counted under. It lives in `master.cfg` on the instance and in the variables of Vercel, nowhere else.
- **An older release cannot reach it.** A client or a game server built before HTTPS refuses an `https://` address: the players and the hosts need a release that speaks it.
- **The domain needs its record.** If the Elastic IP is ever replaced, only the A record changes; everything else holds the domain.
- **One machine, one database, no backup.** A restart cuts the master for a few seconds; the games in progress go on, since no datagram of a game goes through the master.
- **The role of the deployments runs any script on the instance**, because SSM's `AWS-RunShellScript` does. The role reaches only that instance, and only from the approved environment.

## Where to intervene

| You want to… | Look at |
|---|---|
| Change the machine, its disk or its size | `instance_type` in `infra/terraform/variables.tf`; `aws_instance.master` in `master.tf` |
| Open or close a port, or move the API | the rules of `infra/terraform/network.tf`; `master_port` in `variables.tf` and the port in `infra/master/deploy.sh`, kept equal |
| Change the domain of the master | `master_domain` in Terraform, its A record, and `MASTER_URL` of the repository |
| Change what Caddy does | `install_proxy()` in `infra/master/deploy.sh` |
| Change how a release is installed, or the service | `infra/master/deploy.sh`; the download in `infra/release.sh`, which the game machine shares |
| Change how often the master is checked, or when it is restarted | `install_health_check()` in `infra/master/deploy.sh` |
| Run game servers for a LAN | [Game servers for a LAN](/R-Type/deploy-game-servers/) |
| Change who may deploy | the conditions of `aws_iam_role.deploy` in `infra/terraform/github.tf`; the reviewers of the `production` environment in GitHub |
| Change the region | `region` in `variables.tf`, and `AWS_REGION` in the variables of the repository |
| Deploy an older release | run `deploy-master` by hand with its tag |
