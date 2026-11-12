#!/bin/sh
# Prepares, once, what the workflows lan-open and lan-close need, from the
# computer of a member of the team: applies infra/terraform/ again so the
# network rules and the role of the game machine and the role rtype-lan
# exist, sets the two variables of the repository the workflows read, then
# creates the keys of the master of the pool (create-keys.sh). It reads the
# region, the master and its instance from the variables of the repository,
# and refuses a Terraform plan that changes or destroys anything. Every step
# first looks at what exists, so after a failure, or once the master creates
# servers again, you run the same command again.
# Usage: infra/game-servers/prepare.sh [--bucket <bucket>] [--yes]
set -eu

STATE_KEY=r-type/master.tfstate

scripts=$(cd -- "$(dirname -- "$0")" && pwd)
terraform_folder=$scripts/../terraform
bucket=''
assume_yes=0

usage() {
  cat <<'USAGE'
Usage: infra/game-servers/prepare.sh [options]

  --bucket <bucket>   the S3 bucket of the Terraform state; found by itself
                      when one bucket of the account holds the state of the
                      master
  --yes               do not ask before applying
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
      --bucket) bucket=${2:?--bucket needs a value}; shift 2 ;;
      --yes) assume_yes=1; shift ;;
      --help) usage; exit 0 ;;
      *) usage >&2; fail "unknown option: $1" ;;
    esac
  done
}

check_accounts() {
  step 'Accounts'
  for tool in aws terraform gh jq curl; do
    command -v "$tool" > /dev/null 2>&1 || fail "$tool is not installed"
  done
  account=$(aws sts get-caller-identity --query Account --output text) ||
    fail 'the AWS CLI is not logged in (aws login)'
  gh auth status > /dev/null 2>&1 || fail 'the GitHub CLI is not logged in (gh auth login)'
  repository=$(gh repo view --json nameWithOwner --jq .nameWithOwner) ||
    fail 'gh cannot tell which repository this is'
}

# Prints the variable $1 of the repository, and fails when it is not set:
# the setup of the master sets it (infra/bootstrap.sh).
repository_variable() {
  value=$(gh variable list --json name,value --jq ".[] | select(.name == \"$1\") | .value")
  [ -n "$value" ] || fail "the variable $1 of the repository is not set: set up the master first"
  printf '%s' "$value"
}

# What the master runs on today, so the plan never changes its machine.
read_the_master() {
  region=$(repository_variable AWS_REGION)
  master_url=$(repository_variable MASTER_URL)
  instance=$(repository_variable MASTER_INSTANCE_ID)
  instance_type=$(aws ec2 describe-instances --region "$region" --instance-ids "$instance" \
    --query 'Reservations[0].Instances[0].InstanceType' --output text) ||
    fail "AWS does not know the instance $instance in $region"
  printf 'AWS account %s, region %s\nGitHub repository %s\nMaster %s on %s (%s)\n' \
    "$account" "$region" "$repository" "$master_url" "$instance" "$instance_type"
}

# The bucket is the one that holds the state of the master.
find_the_bucket() {
  if [ -n "$bucket" ]; then
    return 0
  fi
  for candidate in $(aws s3api list-buckets --query 'Buckets[].Name' --output text); do
    if aws s3api head-object --bucket "$candidate" --key "$STATE_KEY" > /dev/null 2>&1; then
      [ -z "$bucket" ] || fail "$bucket and $candidate both hold $STATE_KEY: name one with --bucket"
      bucket=$candidate
    fi
  done
  [ -n "$bucket" ] || fail "no bucket of the account holds $STATE_KEY: name it with --bucket"
}

# Prints the resources the plan $1 would change or destroy; a plan that only
# creates prints nothing.
changed_resources() {
  terraform -chdir="$terraform_folder" show -json "$1" | jq -r '
    .resource_changes[]? |
    select(.change.actions | IN(["no-op"], ["create"], ["read"]) | not) |
    "\(.change.actions | join("+")) \(.address)"'
}

apply_infrastructure() {
  step "Infrastructure, state in the bucket $bucket"
  export TF_VAR_master_domain="${master_url#https://}"
  export TF_VAR_region="$region"
  export TF_VAR_github_repository="$repository"
  export TF_VAR_state_bucket="$bucket"
  export TF_VAR_instance_type="$instance_type"
  terraform -chdir="$terraform_folder" init -input=false \
    -backend-config="bucket=$bucket" -backend-config="region=$region"
  plan_folder=$(mktemp -d)
  trap 'rm -rf "$plan_folder"' EXIT
  terraform -chdir="$terraform_folder" plan -input=false -out="$plan_folder/plan"
  changed=$(changed_resources "$plan_folder/plan")
  if [ -n "$changed" ]; then
    printf '%s\n' "$changed" >&2
    fail 'the plan changes or destroys the resources above: nothing applied; read it and apply by hand'
  fi
  confirm 'The plan only creates resources. Apply it?' || fail 'stopped before the apply'
  terraform -chdir="$terraform_folder" apply -input=false "$plan_folder/plan"
}

set_repository_variables() {
  step 'Variables of the repository'
  gh variable set AWS_LAN_ROLE_ARN \
    --body "$(terraform -chdir="$terraform_folder" output -raw lan_role_arn)"
  gh variable set TERRAFORM_STATE_BUCKET --body "$bucket"
  echo 'AWS_LAN_ROLE_ARN and TERRAFORM_STATE_BUCKET are set'
}

# The ports of the pool are the ones the security group of the game machine
# opens, which it holds as tags.
create_keys() {
  step 'Keys of the master of the pool'
  tags=$(aws ec2 describe-security-groups --region "$region" \
    --filters Name=group-name,Values=rtype-game --query 'SecurityGroups[0].Tags' --output json)
  first=$(printf '%s' "$tags" | jq -r '.[] | select(.Key == "first_game_port") | .Value')
  last=$(printf '%s' "$tags" | jq -r '.[] | select(.Key == "last_game_port") | .Value')
  if [ -z "$first" ] || [ -z "$last" ]; then
    fail 'the security group rtype-game does not hold the ports of the pool'
  fi
  # shellcheck disable=SC2046
  AWS_REGION=$region "$scripts/create-keys.sh" "$master_url" $(seq "$first" "$last") ||
    fail 'some keys are missing: run this script again once the master creates servers again'
}

main() {
  read_arguments "$@"
  check_accounts
  read_the_master
  find_the_bucket
  apply_infrastructure
  set_repository_variables
  create_keys
  step 'Done'
  echo 'lan-open and lan-close can run. Open a LAN once before the real one, then close it.'
}

main "$@"
