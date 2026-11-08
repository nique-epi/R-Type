---
title: Deploying the master
description: How a release reaches the master instance on AWS, how to set it up once, and where to change it.
---

The master (see [Master](/R-Type/master/)) runs on one small AWS machine, described with Terraform in `infra/`. Publishing a release installs its server package on that machine, after a person approves it in GitHub; no key to AWS is stored anywhere. This page tells you how that works, how to set it up once and where to change it. It describes what exists today and says plainly what has not been run yet.

| Part | State | Where |
|---|---|---|
| The machine, its address, its network rules and the role of the deployments, in Terraform | Available; not applied to an account yet | `infra/terraform/` |
| The script that installs a release and goes back when the master does not answer | Available | `infra/master/deploy.sh` |
| The workflow that runs the script, on every release or by hand | Available | `.github/workflows/deploy-master.yaml`, called by `release.yaml` |
| The check of the Terraform files and of the scripts, on a pull request | Available | `.github/workflows/infra.yaml` |
| HTTPS through Caddy, for a domain name | Available; not deployed yet | `infra/master/deploy.sh`, `master_domain` in `variables.tf` |
| A backup of the database | Not yet written | |

## What runs on AWS

- **One instance**, `t3.medium` (an x86 machine with 2 vCPU and 4 GiB of memory; `instance_type` in `variables.tf`) running Ubuntu 24.04, with an 8 GiB disk. The package of a release is built on Ubuntu 22.04 for x86, and its C library must be at least that of the build, so a machine with an older one, such as Amazon Linux 2023, cannot run it.
- **One address that does not change**, an Elastic IP (`aws_eip.master`). The A record of the domain of the master (`master_domain`) points at it, so Terraform refuses to destroy it (`prevent_destroy`).
- **Two open ports**, from anywhere: 443, the HTTPS API, and 80, which only answers the certificate challenges of Let's Encrypt and sends the rest to HTTPS. The master listens on 8080 (`master_port`), which no rule opens. There is no SSH and no key pair: the machine is reached through AWS Systems Manager (SSM), which needs no open port.
- **Caddy**, the package of Ubuntu, in front of the master: it answers HTTPS for `master_domain` with a certificate of Let's Encrypt it gets at the first deployment and renews by itself, and forwards each request to `127.0.0.1:8080`.
- **One systemd service**, `rtype-master`, which runs `r-type_server --mode master -p 8080` as the user `rtype`, with the database in `/var/lib/rtype/r-type_master.db`. It writes no journal file; its lines go to `journalctl -u rtype-master`. The port in the unit and `master_port` must stay equal.
- **A CPU on credits.** A `t3` machine earns CPU credits at a baseline of 20% of each vCPU and spends them when it works harder. The instance is set to `cpu_credits = "standard"`: once the credits are gone, the CPU is slowed down, and the bill never exceeds the price of the instance. The master is an HTTP API on a SQLite file, which stays far below that baseline. A machine whose CPU never slows down (`c7i-flex.large`, `c5.large`) costs about twice as much.

The size is **provisional**: measure the free memory with `free -m` and the credits left (`CPUCreditBalance` in CloudWatch) on the running master before keeping it. The cost is an estimate from public prices, **not measured**: about 37 USD a month, of which about 33 for the machine.

## The deployment: `deploy-master`

The workflow `deploy-master` runs in the GitHub environment `production`, so it waits for the reviewer that environment names. It runs after the `publish` job of `release.yaml` on every tag `v*`, and by hand (Actions, `deploy-master`, Run workflow, with a tag such as `v0.1.0`) to redeploy or to go back to an older release. Only one deployment runs at a time.

| Step | What happens |
|---|---|
| 1 | The tag is checked: it must read `vX.Y.Z`. |
| 2 | The job assumes the AWS role `rtype-master-deploy` with a short-lived token of GitHub (OpenID Connect). The role trusts only this repository's `production` environment, and may only run a script on this one instance. |
| 3 | The job reads the domain of `MASTER_URL`, which must be `https://<domain>`, with no port and no path. |
| 4 | The job sends `infra/master/deploy.sh`, preceded by the lines `VERSION=<x.y.z>` and `DOMAIN=<domain>`, to the instance through SSM Run Command, and waits for it. |
| 5 | The job calls `GET /api/health` on `MASTER_URL` from the outside, for up to a minute, the time Let's Encrypt takes to sign the first certificate. |

`deploy.sh` runs as root on the instance:

1. It downloads `R-Type-server-<version>-Linux.tar.gz` from the public release, and unpacks it in `/opt/rtype/releases/`. A release that does not exist stops the script here, before anything changed.
2. It writes the unit, points `/opt/rtype/current` at the new release and restarts the service.
3. It waits up to 20 seconds for `GET /api/health` to answer 200. When it does not, it prints the last 30 lines of the journal, points `current` back at the previous release, restarts and fails.
4. It deletes the releases other than the new one and the previous one.
5. It installs Caddy once, writes `/etc/caddy/Caddyfile` for `DOMAIN`, checks it with `caddy validate` and reloads Caddy.

Running the same tag again installs nothing new and restarts the master. The check only reads the status code: `MASTER_VERSION` is written by hand in `src/master/MasterConstants.hpp`, it does not follow the tag.

Going back after the database was migrated is not guaranteed: the new master migrates the database when it starts, and an older one may not read a newer schema.

## Setting it up once

A member of the team does this once, with their own AWS, GitHub and Vercel accounts; nothing here runs by itself. The steps are in the [README of `infra/`](https://github.com/nique-epi/R-Type/blob/main/infra/README.md), in this order:

1. Create the S3 bucket of the Terraform state, then `terraform init`, `plan` and `apply` from `infra/terraform/`, with `master_domain` set to the domain of the master.
2. At the DNS provider of that domain, point an `A` record at the `master_public_ip` output.
3. Create the four variables of the repository from the outputs of Terraform: `AWS_REGION`, `AWS_DEPLOY_ROLE_ARN`, `MASTER_INSTANCE_ID` and `MASTER_URL`. They are variables, not secrets: no secret is needed in GitHub.
4. Create the environment `production` with at least one required reviewer: the deployment waits for it, and the AWS role trusts only that environment.
5. Run `deploy-master` by hand with the tag of an existing release, and check that the instance is `Online` in SSM and that `GET /api/health` answers over HTTPS.
6. Give the `master_url` output to the web site, the game servers and the client.

The README also lists the symptoms of each step that goes wrong.

## Giving the address to the web site

The web site calls the master from its own server, at `MASTER_URL` (see [Master](/R-Type/master/#the-web-site)), and Vercel hosts it. Set `MASTER_URL` to the `master_url` output in the Vercel project, for the Production and Preview environments, then deploy the site again: a variable only reaches the deployments made after it changed, and `next.config.ts` reads it at the build too. The browser never calls the master, so the master needs no CORS.

## Limits

- **HTTPS ends at Caddy.** Between Caddy and the master, on the machine, requests go in plain HTTP to `127.0.0.1:8080`. The master reads the address of each client in the `X-Forwarded-For` header Caddy writes, and trusts it only from `127.0.0.1` (see [Master](/R-Type/master/#the-http-api-masterhttpserver)).
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
| Change how a release is installed, or the service | `infra/master/deploy.sh` |
| Change who may deploy | the conditions of `aws_iam_role.deploy` in `infra/terraform/github.tf`; the reviewers of the `production` environment in GitHub |
| Change the region | `region` in `variables.tf`, and `AWS_REGION` in the variables of the repository |
| Deploy an older release | run `deploy-master` by hand with its tag |
