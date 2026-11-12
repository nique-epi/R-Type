# Infrastructure of the master and of the game servers

This folder puts the master (`r-type_server --mode master`) on a small AWS machine and installs each release on it. It holds:

| Path | What it is |
|---|---|
| `terraform/` | the machine, its address, its network rules and the role the deployments use, described with Terraform |
| `bootstrap.sh` | the script that does the whole setup below in one run, from your computer |
| `master/deploy.sh` | the script that installs a release on the machine, goes back when the master does not answer, and puts Caddy in front of it for HTTPS; it also installs the timer that restarts a master that stopped answering, and the test game server |
| `release.sh` | what the scripts that install a release share: the check of the version and the download |
| `run-on-instance.sh` | runs a script on an instance through SSM; the workflows call it |
| `terraform-game/` | the game machine of a LAN, with a Terraform state of its own; the workflows `lan-open` and `lan-close` apply and destroy it |
| `game-servers/` | the scripts of the pool of game servers: `install.sh`, the autoscaler `autoscale.sh` and its settings, `listed-servers.sh` and `create-keys.sh` |
| `../.github/workflows/lan-open.yaml`, `lan-close.yaml` | the workflows that create and destroy the game machine, by hand |
| `../.github/workflows/deploy-master.yaml` | the workflow that runs the script, on every release or by hand |
| `../.github/workflows/infra.yaml` | the check of these files on a pull request |

Nothing here runs by itself the first time: a member of the team sets it up once, with the steps below. How the deployment works and where to change it is told in the documentation page [Deploying the master](https://nique-epi.github.io/R-Type/deploy-master/). The game servers of a LAN, their workflows and what to prepare once are told in [Game servers for a LAN](https://nique-epi.github.io/R-Type/deploy-game-servers/).

## What you get

- One `t3.medium` instance (x86, 2 vCPU, 4 GiB of memory, Ubuntu 24.04, 8 GiB disk) with an Elastic IP that does not change, about 37 USD a month (an estimate from public prices, not measured). Its CPU runs on credits in "standard" mode: it is slowed down when the credits are gone and never billed more.
- Two open ports: 443, the HTTPS API of the master through Caddy, and 80, which only serves the certificate challenges of Let's Encrypt and sends the rest to HTTPS. The master listens on 8080, which no rule opens: only Caddy, on the machine, reaches it. There is no SSH and no key pair: the machine is reached through AWS Systems Manager (SSM).
- A certificate of Let's Encrypt for the domain of the master, which Caddy gets at the first deployment and renews by itself.
- A deployment on every release (`v*` tag), after a person approves it in GitHub. No AWS key is stored in the repository or in GitHub: the workflow gets a short-lived token from GitHub (OpenID Connect) and trades it for a role.

## What you need

- An AWS account, and the AWS CLI logged in to it (`aws sts get-caller-identity` answers).
- A domain name whose DNS you can change, for the master (`master_domain`, such as `master.example.org`). Let's Encrypt gives no certificate for a bare IP address.
- Terraform 1.10 or later (`terraform version`).
- The GitHub CLI logged in to the repository with the right to write its variables (`gh auth status`), or the same rights in the web interface.
- The right to create an environment in the repository settings.
- A Vercel project for the web site, if the site must read the master.

## Setting it up with one script

From the root of the repository, with the tools of the list above installed and logged in:

```bash
infra/bootstrap.sh --domain <domain of the master> --bucket rtype-tfstate-<suffix>
```

It asks which AWS account and repository it is about, creates the bucket, applies Terraform after you read the plan, sets the six variables, creates the environment `production` with you as its required reviewer, then waits for the A record of the domain (it prints the address to give it) and polls until it answers, for up to 30 minutes. `--deploy v0.3.0` also starts `deploy-master` for that release once the record answers, and `--yes` skips the two questions. Every step first looks at what exists, so after a failure you run the same command again. It does not publish a release, and it does not set the address in Vercel, the game servers or the client (step 6 below).

## Setting it up by hand, in this order

The steps the script runs, one by one.

Run the commands from `infra/terraform/`, with the name `rtype-tfstate-<suffix>` replaced by a bucket name of your choice: it must be unique in the world.

### 1. The bucket of the Terraform state

```bash
aws s3api create-bucket --bucket rtype-tfstate-<suffix> --region eu-west-1 \
  --create-bucket-configuration LocationConstraint=eu-west-1
aws s3api put-bucket-versioning --bucket rtype-tfstate-<suffix> \
  --versioning-configuration Status=Enabled
aws s3api put-public-access-block --bucket rtype-tfstate-<suffix> \
  --public-access-block-configuration BlockPublicAcls=true,IgnorePublicAcls=true,BlockPublicPolicy=true,RestrictPublicBuckets=true
```

The bucket keeps the Terraform state, so every member of the team can apply from their own computer. It is created by hand because Terraform needs it before it can create anything.

### 2. The infrastructure

```bash
terraform init -backend-config="bucket=rtype-tfstate-<suffix>" -backend-config="region=eu-west-1"
terraform plan -var master_domain=<domain of the master> -var state_bucket=rtype-tfstate-<suffix>
terraform apply -var master_domain=<domain of the master> -var state_bucket=rtype-tfstate-<suffix>
```

Both can also go in a `terraform.tfvars` file next to the `.tf` files (`master_domain = "master.example.org"`, `state_bucket = "rtype-tfstate-<suffix>"`), so they are not typed each time. Read the plan before the `apply`: it must only create resources (23 of them, plus the reads of the account, of the default network and of the Ubuntu image), never destroy one. Terraform creates the instance, its address, its network rules, the role of the instance, the identity provider of GitHub and the role of the deployments; and, for the game machine of a LAN, its network rules, its role and the role of the workflows that create and destroy it. The game machine itself is not created here. On an infrastructure applied before the game machine, the plan creates those 11 resources and changes nothing else.

On an infrastructure applied before HTTPS, the plan replaces the rule of port 8080 by the rules of ports 80 and 443: one resource destroyed, two created. That rule is the only one it may destroy.

An AWS account has one identity provider per URL. If the account already has the one of GitHub (`token.actions.githubusercontent.com`), the `apply` fails; import it instead of creating it:

```bash
terraform import aws_iam_openid_connect_provider.github \
  arn:aws:iam::<account id>:oidc-provider/token.actions.githubusercontent.com
```

The Elastic IP cannot be destroyed (`prevent_destroy`): the A record of the domain points at it. To delete it on purpose, remove that block first.

### 2b. The A record of the domain

At the DNS provider of the domain, create an `A` record for `master_domain` pointing at `terraform output -raw master_public_ip`. Let's Encrypt checks that the domain reaches the machine before it signs the certificate, so the first deployment fails until the record answers:

```bash
dig +short <domain of the master>
```

must print the address of the output.

### 3. The variables of the repository

Create them in Settings, Secrets and variables, Actions, tab **Variables** (not Secrets: none is sensitive, and the workflow reads them as `vars.*`). No secret is needed in GitHub.

| Variable | Value | Read by | If it is missing |
|---|---|---|---|
| `AWS_REGION` | `eu-west-1`, the `region` of `terraform/variables.tf` | `deploy-master.yaml`, to log in to AWS | the job stops at the login step |
| `AWS_DEPLOY_ROLE_ARN` | `terraform output -raw deploy_role_arn` | `deploy-master.yaml`, the role to assume | the job stops at the login step |
| `MASTER_INSTANCE_ID` | `terraform output -raw instance_id` | `deploy-master.yaml`, the only instance the script is sent to | the job stops when it sends the script |
| `MASTER_URL` | `terraform output -raw master_url` (`https://<master_domain>`, with no port and no path) | `deploy-master.yaml`: the domain Caddy answers for, and the check from outside after a deployment | the job stops before it sends the script |
| `AWS_LAN_ROLE_ARN` | `terraform output -raw lan_role_arn` | `lan-open.yaml` and `lan-close.yaml`, the role to assume | the job stops at the login step |
| `TERRAFORM_STATE_BUCKET` | `rtype-tfstate-<suffix>`, the bucket of step 1 | `lan-open.yaml` and `lan-close.yaml`, where the state of the game machine is kept | the job stops at `terraform init` |

```bash
gh variable set AWS_REGION --body eu-west-1
gh variable set MASTER_INSTANCE_ID --body "$(terraform output -raw instance_id)"
gh variable set AWS_DEPLOY_ROLE_ARN --body "$(terraform output -raw deploy_role_arn)"
gh variable set MASTER_URL --body "$(terraform output -raw master_url)"
gh variable set AWS_LAN_ROLE_ARN --body "$(terraform output -raw lan_role_arn)"
gh variable set TERRAFORM_STATE_BUCKET --body rtype-tfstate-<suffix>
```

If you changed `region` in `variables.tf`, give `AWS_REGION` the same value.

### 4. The environment `production`

In Settings, Environments, create `production` and name at least one **required reviewer**. Two things depend on it:

- the deployment waits for that reviewer before it touches the master;
- the AWS role trusts only `repo:nique-epi/R-Type:environment:production`.

A job that names an environment that does not exist creates it with no reviewer, so without this step the deployment would not wait for anybody.

### 5. The first deployment

In Actions, run `deploy-master` by hand (Run workflow) with the tag of an existing release, such as `v0.1.0`, and approve it. Then check:

```bash
aws ssm describe-instance-information
curl "$(terraform output -raw master_url)/api/health"
```

The first command must list the instance as `Online`; the second must answer `{"version":"0.1.0"}`, over HTTPS. The release deployed must be one whose package speaks HTTPS to the master; the script only installs Caddy, whatever the release. After that, every release is deployed on its own, after the approval.

### 6. The address of the master, where it is read

Give the `master_url` output to everything that talks to the master:

| Where | Setting | What to do |
|---|---|---|
| The web site on Vercel | `MASTER_URL`, read by `web/src/config/env.config.ts` | set it in the Vercel project for Production and Preview, then deploy the site again: a variable only reaches the deployments made after it changed |
| The game servers | `master-url` in `server.cfg` | put the same address |
| The client | `master-url`, or the master chosen in the game | put the same address |

The players and the hosts need a release of the game that speaks HTTPS: an older client or game server refuses an `https://` address. If the Elastic IP is ever replaced, only the A record changes: everything else holds the domain.

## Day to day

| You want to… | Do |
|---|---|
| Deploy a release | publish its tag; the `deploy-master` job waits for the approval |
| Deploy an older release, or redeploy | Actions, `deploy-master`, Run workflow, with its tag |
| Read the log of the master | `aws ssm start-session --target <instance id>` (it needs the Session Manager plugin of the AWS CLI), then `journalctl -u rtype-master` |
| Change the size of the machine | `instance_type` in `terraform/variables.tf`, then `terraform apply` |
| Check the memory and the CPU of the machine | on the machine, `free -m`; the credits left are `CPUCreditBalance` in CloudWatch: a balance that stays near 0 means the CPU is slowed down, so use a bigger machine |

## When it does not work

| Symptom | Cause to look at |
|---|---|
| `Not authorized to perform sts:AssumeRoleWithWebIdentity` | the `production` environment does not exist or the job does not use it; or GitHub changed the `sub` claim of the token and the condition in `terraform/github.tf` no longer matches it. Print the claim of the failing job and compare |
| The job stops at "Run the deployment on the instance" with an invalid instance | `MASTER_INSTANCE_ID` is missing or belongs to another account or region |
| The instance is not listed by `describe-instance-information` | the SSM agent is not running on the image, or the instance has no outbound access; look at the system log of the instance in the console |
| The script prints "the master of X does not answer" and "went back to ..." | the new release does not start: read its log with `journalctl -u rtype-master` |
| The script fails on the download | the release `v<X>` does not exist, or has no `R-Type-server-<X>-Linux.tar.gz` |
| The job stops at "Read the domain of the master" | `MASTER_URL` is not `https://<domain>`: no `http://`, no port, no path |
| The script fails at `caddy validate` or Caddy does not start | read `journalctl -u caddy` on the machine |
| The check from outside fails but the script succeeded | the A record does not point at the Elastic IP yet, or Let's Encrypt refused the certificate: `journalctl -u caddy` says which; ports 80 and 443 must be open |
| Going back does not restore the master | the new master migrated the database, and the older one cannot read the newer schema |

## Limits

- **HTTPS ends at Caddy.** Between Caddy and the master, on the machine itself, requests go in plain HTTP to 127.0.0.1:8080. The master reads the address of each client in the `X-Forwarded-For` header Caddy writes, and trusts that header only from 127.0.0.1.
- **One machine, one database, no backup.** A restart cuts the master for a few seconds; the games in progress go on, since no datagram of a game goes through the master.
- **The role of the deployments runs any script on the machine**, because SSM's `AWS-RunShellScript` does. The role reaches only that machine, and only from the approved environment.
- **The first deployment has not run yet**, and the return to the previous release when the master does not answer has not been tested end to end.
