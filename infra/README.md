# Infrastructure of the master

This folder puts the master (`r-type_server --mode master`) on a small AWS machine and installs each release on it. It holds:

| Path | What it is |
|---|---|
| `terraform/` | the machine, its address, its network rule and the role the deployments use, described with Terraform |
| `master/deploy.sh` | the script that installs a release on the machine and goes back when the master does not answer |
| `../.github/workflows/deploy-master.yaml` | the workflow that runs the script, on every release or by hand |
| `../.github/workflows/infra.yaml` | the check of these files on a pull request |

Nothing here runs by itself the first time: a member of the team sets it up once, with the steps below. How the deployment works and where to change it is told in the documentation page [Deploying the master](https://nique-epi.github.io/R-Type/deploy-master/).

## What you get

- One `t3.medium` instance (x86, 2 vCPU, 4 GiB of memory, Ubuntu 24.04, 8 GiB disk) with an Elastic IP that does not change, about 37 USD a month (an estimate from public prices, not measured). Its CPU runs on credits in "standard" mode: it is slowed down when the credits are gone and never billed more.
- One open port, 8080, the HTTP API of the master. There is no SSH and no key pair: the machine is reached through AWS Systems Manager (SSM).
- A deployment on every release (`v*` tag), after a person approves it in GitHub. No AWS key is stored in the repository or in GitHub: the workflow gets a short-lived token from GitHub (OpenID Connect) and trades it for a role.

## What you need

- An AWS account, and the AWS CLI logged in to it (`aws sts get-caller-identity` answers).
- Terraform 1.10 or later (`terraform version`).
- The GitHub CLI logged in to the repository with the right to write its variables (`gh auth status`), or the same rights in the web interface.
- The right to create an environment in the repository settings.
- A Vercel project for the web site, if the site must read the master.

## Setting it up, in this order

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
terraform plan
terraform apply
```

Read the plan before the `apply`: it must only create resources (11 of them, plus the reads of the default network and of the Ubuntu image), never destroy one. Terraform creates the instance, its address, its network rule, the role of the instance, the identity provider of GitHub and the role of the deployments.

An AWS account has one identity provider per URL. If the account already has the one of GitHub (`token.actions.githubusercontent.com`), the `apply` fails; import it instead of creating it:

```bash
terraform import aws_iam_openid_connect_provider.github \
  arn:aws:iam::<account id>:oidc-provider/token.actions.githubusercontent.com
```

The Elastic IP cannot be destroyed (`prevent_destroy`): the game servers, the client and the web site all hold its address. To delete it on purpose, remove that block first.

### 3. The variables of the repository

Create them in Settings, Secrets and variables, Actions, tab **Variables** (not Secrets: none is sensitive, and the workflow reads them as `vars.*`). No secret is needed in GitHub.

| Variable | Value | Read by | If it is missing |
|---|---|---|---|
| `AWS_REGION` | `eu-west-1`, the `region` of `terraform/variables.tf` | `deploy-master.yaml`, to log in to AWS | the job stops at the login step |
| `AWS_DEPLOY_ROLE_ARN` | `terraform output -raw deploy_role_arn` | `deploy-master.yaml`, the role to assume | the job stops at the login step |
| `MASTER_INSTANCE_ID` | `terraform output -raw instance_id` | `deploy-master.yaml`, the only instance the script is sent to | the job stops when it sends the script |
| `MASTER_URL` | `terraform output -raw master_url` (`http://<Elastic IP>:8080`) | `deploy-master.yaml`, the check from outside after a deployment | the check fails, although the master may be running |

```bash
gh variable set AWS_REGION --body eu-west-1
gh variable set MASTER_INSTANCE_ID --body "$(terraform output -raw instance_id)"
gh variable set AWS_DEPLOY_ROLE_ARN --body "$(terraform output -raw deploy_role_arn)"
gh variable set MASTER_URL --body "$(terraform output -raw master_url)"
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

The first command must list the instance as `Online`; the second must answer `{"version":"0.1.0"}`. After that, every release is deployed on its own, after the approval.

### 6. The address of the master, where it is read

Give the `master_url` output to everything that talks to the master:

| Where | Setting | What to do |
|---|---|---|
| The web site on Vercel | `MASTER_URL`, read by `web/src/config/env.config.ts` | set it in the Vercel project for Production and Preview, then deploy the site again: a variable only reaches the deployments made after it changed |
| The game servers | `master-url` in `server.cfg` | put the same address |
| The client | `master-url`, or the master chosen in the game | put the same address |

If the Elastic IP is ever replaced, change these three places and `MASTER_URL` of the repository together.

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
| The check from outside fails but the script succeeded | `MASTER_URL` is wrong, or port 8080 is blocked between the runner and the machine |
| Going back does not restore the master | the new master migrated the database, and the older one cannot read the newer schema |

## Limits

- **The API is not encrypted.** The key of a game server travels in an `Authorization: Bearer` header over plain HTTP, because the HTTP library is built without TLS. HTTPS needs a change of the build, a certificate and a domain name.
- **One machine, one database, no backup.** A restart cuts the master for a few seconds; the games in progress go on, since no datagram of a game goes through the master.
- **The role of the deployments runs any script on the machine**, because SSM's `AWS-RunShellScript` does. The role reaches only that machine, and only from the approved environment.
- **The first deployment has not run yet**, and the return to the previous release when the master does not answer has not been tested end to end.
