#!/bin/sh
# Runs a script on an instance through SSM Run Command, prints what it wrote
# and fails when it failed. The workflows that deploy the master and that
# open and close a LAN call it.
# Usage: infra/run-on-instance.sh <instance id> <script file>
set -eu

instance=$1
script=$2
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT

jq -n --rawfile script "$script" '{commands: [$script]}' > "$work/parameters.json"
command_id=$(aws ssm send-command \
  --instance-ids "$instance" \
  --document-name AWS-RunShellScript \
  --parameters "file://$work/parameters.json" \
  --timeout-seconds 300 \
  --query Command.CommandId --output text)
# The invocation can take a moment to exist after send-command.
sleep 3
aws ssm wait command-executed --command-id "$command_id" --instance-id "$instance" || true
invocation=$(aws ssm get-command-invocation \
  --command-id "$command_id" --instance-id "$instance" --output json)
printf '%s' "$invocation" | jq -r .StandardOutputContent
printf '%s' "$invocation" | jq -r .StandardErrorContent >&2
printf '%s' "$invocation" | jq -e '.Status == "Success"' > /dev/null
