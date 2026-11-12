#!/bin/sh
# Sets up the infrastructure of the master from nothing, once, from the
# computer of a member of the team: the bucket of the Terraform state, the
# infrastructure, the variables and the environment of the repository, then
# the wait for the A record of the domain. Each step first looks at what
# exists, so running the script again only does what is left.
#
#   infra/bootstrap.sh --domain master.example.org --bucket rtype-tfstate-me
#
# It needs the AWS CLI, Terraform, the GitHub CLI and dig, the first three
# logged in to the right accounts. See infra/README.md.
set -eu

DEFAULT_REGION=eu-west-1
DNS_RESOLVER=1.1.1.1
DNS_POLL_SECONDS=15
DNS_WAIT_LIMIT_SECONDS=1800

domain=''
bucket=''
region=$DEFAULT_REGION
tag=''
assume_yes=0

usage() {
  cat <<'USAGE'
Usage: infra/bootstrap.sh --domain <domain> --bucket <bucket> [options]

  --domain <domain>   the domain name of the master, such as master.example.org
  --bucket <bucket>   the S3 bucket of the Terraform state; its name is unique
                      in the world, such as rtype-tfstate-<your name>
  --region <region>   the AWS region (default: eu-west-1)
  --deploy <tag>      once the domain answers, run deploy-master for this
                      release, such as v0.3.0; it waits for your approval
  --yes               do not ask before applying; the account is not asked
                      about either
  --help              print this text
USAGE
}

fail() {
  printf 'error: %s\n' "$*" >&2
  exit 1
}

step() {
  printf '\n== %s\n' "$*"
}

confirm() {
  if [ "$assume_yes" = 1 ]; then
    return 0
  fi
  printf '%s [y/N] ' "$1"
  read -r answer
  case $answer in
    y | Y | yes | YES) return 0 ;;
    *) return 1 ;;
  esac
}

read_arguments() {
  while [ $# -gt 0 ]; do
    case $1 in
      --domain) domain=${2:?--domain needs a value}; shift 2 ;;
      --bucket) bucket=${2:?--bucket needs a value}; shift 2 ;;
      --region) region=${2:?--region needs a value}; shift 2 ;;
      --deploy) tag=${2:?--deploy needs a value}; shift 2 ;;
      --yes) assume_yes=1; shift ;;
      --help) usage; exit 0 ;;
      *) usage >&2; fail "unknown option: $1" ;;
    esac
  done
}

# Same rules as the variable master_domain of Terraform and the workflow
# deploy-master, so a name refused there is refused here first.
check_inputs() {
  [ -n "$domain" ] || fail '--domain is required'
  [ -n "$bucket" ] || fail '--bucket is required'
  if ! printf '%s' "$domain" | grep -Eq '^([a-z0-9]([a-z0-9-]*[a-z0-9])?\.)+[a-z]{2,}$'; then
    fail "not a domain name in lowercase: $domain"
  fi
  if ! printf '%s' "$bucket" | grep -Eq '^[a-z0-9][a-z0-9.-]{1,61}[a-z0-9]$'; then
    fail "not a bucket name (3 to 63 lowercase letters, digits, dots, dashes): $bucket"
  fi
  if [ -n "$tag" ] && ! printf '%s' "$tag" | grep -Eq '^v[0-9]+\.[0-9]+\.[0-9]+$'; then
    fail "not a version tag: $tag"
  fi
}

require_tools() {
  for tool in aws terraform gh dig; do
    command -v "$tool" > /dev/null 2>&1 || fail "$tool is not installed"
  done
}

# Everything below runs from infra/, wherever the script was called from.
go_to_infra() {
  cd -- "$(dirname -- "$0")"
}

# Prints what the sub claim of the GitHub tokens of the repository starts
# with: with immutable subjects, GitHub names the owner and the repository by
# their ids, and the trust of the deployment role must read the same.
subject_prefix() {
  answer=$(gh api "repos/$repository/actions/oidc/customization/sub" \
    --jq '"\(.use_default) \(.sub_claim_prefix // "")"') ||
    fail 'gh cannot read the OIDC subject of the repository'
  use_default=${answer%% *}
  prefix=${answer#* }
  [ "$use_default" = true ] ||
    fail 'the repository has a custom OIDC subject template, which the trust of the deployment role does not read'
  printf '%s' "${prefix:-repo:$repository}"
}

# Terraform reads TF_VAR_<name> for every command, import included.
export_terraform_variables() {
  prefix=$(subject_prefix)
  export TF_VAR_master_domain="$domain"
  export TF_VAR_region="$region"
  export TF_VAR_github_subject_prefix="$prefix"
  export TF_VAR_state_bucket="$bucket"
}

check_accounts() {
  step 'Accounts'
  account=$(aws sts get-caller-identity --query Account --output text) ||
    fail 'the AWS CLI is not logged in (aws sts get-caller-identity fails)'
  gh auth status > /dev/null 2>&1 || fail 'the GitHub CLI is not logged in (gh auth login)'
  repository=$(gh repo view --json nameWithOwner --jq .nameWithOwner) ||
    fail 'gh cannot tell which repository this is'
  printf 'AWS account %s, region %s\nGitHub repository %s\nDomain %s\n' \
    "$account" "$region" "$repository" "$domain"
  confirm 'Create the infrastructure of the master here?' || fail 'stopped, nothing was changed'
}

create_state_bucket() {
  step "Bucket of the Terraform state: $bucket"
  if aws s3api head-bucket --bucket "$bucket" > /dev/null 2>&1; then
    echo 'the bucket exists'
  elif [ "$region" = us-east-1 ]; then
    aws s3api create-bucket --bucket "$bucket" --region "$region" > /dev/null
  else
    aws s3api create-bucket --bucket "$bucket" --region "$region" \
      --create-bucket-configuration "LocationConstraint=$region" > /dev/null
  fi
  aws s3api put-bucket-versioning --bucket "$bucket" \
    --versioning-configuration Status=Enabled
  aws s3api put-public-access-block --bucket "$bucket" \
    --public-access-block-configuration \
    BlockPublicAcls=true,IgnorePublicAcls=true,BlockPublicPolicy=true,RestrictPublicBuckets=true
}

# An AWS account has one identity provider of GitHub; when the account
# already has it, Terraform adopts it instead of failing to create it.
adopt_github_provider() {
  existing=$(aws iam list-open-id-connect-providers --output text \
    --query "OpenIDConnectProviderList[?ends_with(Arn, 'oidc-provider/token.actions.githubusercontent.com')].Arn")
  if [ -z "$existing" ]; then
    return 0
  fi
  if terraform -chdir=terraform state list | grep -Fxq aws_iam_openid_connect_provider.github; then
    return 0
  fi
  echo "the account has the identity provider of GitHub: $existing"
  terraform -chdir=terraform import -input=false \
    aws_iam_openid_connect_provider.github "$existing"
}

apply_infrastructure() {
  step 'Infrastructure'
  terraform -chdir=terraform init -input=false \
    -backend-config="bucket=$bucket" -backend-config="region=$region"
  adopt_github_provider
  plan_directory=$(mktemp -d)
  trap 'rm -rf "$plan_directory"' EXIT
  terraform -chdir=terraform plan -input=false -out="$plan_directory/plan"
  echo 'Read the plan: on a new account it creates resources and destroys none.'
  confirm 'Apply this plan?' || fail 'stopped before the apply'
  terraform -chdir=terraform apply -input=false "$plan_directory/plan"
}

set_repository_variables() {
  step 'Variables of the repository'
  instance_id=$(terraform -chdir=terraform output -raw instance_id)
  deploy_role_arn=$(terraform -chdir=terraform output -raw deploy_role_arn)
  master_url=$(terraform -chdir=terraform output -raw master_url)
  lan_role_arn=$(terraform -chdir=terraform output -raw lan_role_arn)
  gh variable set AWS_REGION --body "$region"
  gh variable set MASTER_INSTANCE_ID --body "$instance_id"
  gh variable set AWS_DEPLOY_ROLE_ARN --body "$deploy_role_arn"
  gh variable set MASTER_URL --body "$master_url"
  gh variable set AWS_LAN_ROLE_ARN --body "$lan_role_arn"
  gh variable set TERRAFORM_STATE_BUCKET --body "$bucket"
}

# The deployment waits for a required reviewer, and the AWS role trusts only
# this environment. A job that names a missing environment creates it with no
# reviewer, so the environment is created here, with you as its reviewer.
create_production_environment() {
  step 'Environment production'
  reviewer_rules=$(gh api "repos/$repository/environments/production" \
    --jq '[.protection_rules[]? | select(.type == "required_reviewers")] | length' \
    2> /dev/null || echo 0)
  if [ "$reviewer_rules" -gt 0 ]; then
    echo 'the environment production already asks for a reviewer'
    return 0
  fi
  user_id=$(gh api user --jq .id)
  printf '{"reviewers":[{"type":"User","id":%s}]}' "$user_id" |
    gh api --method PUT "repos/$repository/environments/production" --input - > /dev/null ||
    fail 'GitHub refused the environment: required reviewers need a public repository or a paid plan'
  echo 'created, with you as the required reviewer; add the others in Settings, Environments'
}

# Let's Encrypt signs the certificate only once the domain reaches the
# machine, so the first deployment waits for this record.
wait_for_the_record() {
  step "A record of $domain"
  address=$(terraform -chdir=terraform output -raw master_public_ip)
  printf 'At the DNS provider, create an A record: %s -> %s\n' "$domain" "$address"
  waited=0
  while [ "$waited" -lt "$DNS_WAIT_LIMIT_SECONDS" ]; do
    if dig +short A "$domain" "@$DNS_RESOLVER" | grep -Fxq "$address"; then
      echo 'the record answers'
      return 0
    fi
    sleep "$DNS_POLL_SECONDS"
    waited=$((waited + DNS_POLL_SECONDS))
  done
  fail "no A record after $DNS_WAIT_LIMIT_SECONDS seconds; create it, then run this script again, it skips what is done"
}

start_deployment() {
  if [ -z "$tag" ]; then
    return 0
  fi
  step "Deployment of $tag"
  gh workflow run deploy-master.yaml --field "tag=$tag"
  echo 'started; approve it in Actions, deploy-master'
}

print_next_steps() {
  step 'Done'
  cat <<NEXT
Check the master once a release that speaks HTTPS is deployed:
  curl $master_url/api/health

Then give $master_url to everything that talks to the master:
  - the web site on Vercel: MASTER_URL, Production and Preview, then deploy again
  - the game servers: master-url in server.cfg
  - the client: master-url
NEXT
}

main() {
  read_arguments "$@"
  check_inputs
  require_tools
  go_to_infra
  check_accounts
  export_terraform_variables
  create_state_bucket
  apply_infrastructure
  set_repository_variables
  create_production_environment
  wait_for_the_record
  start_deployment
  print_next_steps
}

main "$@"
