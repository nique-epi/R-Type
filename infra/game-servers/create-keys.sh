#!/bin/sh
# Creates, once, the key of the master of each server of the pool, and keeps
# it encrypted in AWS Parameter Store under /rtype/game/<port>/master-key,
# where the game machine reads it when it starts. A port that has its key is
# left as it is, and no key is ever printed. The master creates 5 servers an
# hour for one address: when it refuses, the script says which ports are
# left, and is run again an hour later. Run it from a computer whose AWS CLI
# is logged in.
# Usage: infra/game-servers/create-keys.sh <url of the master> <port>...
set -eu

usage() {
  echo 'usage: create-keys.sh <url of the master> <port>...' >&2
  exit 2
}

has_key() {
  aws ssm get-parameter --name "/rtype/game/$1/master-key" > /dev/null 2>&1
}

# Asks the master for a server named after the port $1 and writes its answer
# to the file $2. Prints the HTTP status.
create_server() {
  curl --silent --show-error --max-time 10 --output "$2" --write-out '%{http_code}' \
    --request POST "$master/api/servers" \
    --header 'Content-Type: application/json' \
    --data "{\"name\":\"LAN $1\"}"
}

# Keeps the key of the answer $2 for the port $1. The key goes through a
# file, never through the arguments of a command.
keep_key() {
  jq --arg name "/rtype/game/$1/master-key" \
    '{Name: $name, Type: "SecureString", Value: .key}' "$2" > "$work/parameter.json"
  aws ssm put-parameter --cli-input-json "file://$work/parameter.json" > /dev/null
}

main() {
  [ "$#" -ge 2 ] || usage
  master=$1
  shift
  # Without this, a logged out AWS CLI reads as "no key yet": a server would
  # be created on the master and its key lost.
  if ! aws sts get-caller-identity > /dev/null; then
    echo 'the AWS CLI is not logged in' >&2
    exit 1
  fi
  work=$(mktemp -d)
  trap 'rm -rf "$work"' EXIT
  umask 077

  while [ "$#" -gt 0 ]; do
    if has_key "$1"; then
      echo "port $1 has its key"
      shift
      continue
    fi
    status=$(create_server "$1" "$work/created.json")
    case $status in
      201) ;;
      429)
        echo "the master creates no more servers this hour; run again in an hour for: $*" >&2
        exit 1
        ;;
      *)
        echo "the master answered $status for port $1" >&2
        exit 1
        ;;
    esac
    keep_key "$1" "$work/created.json"
    echo "port $1: key created and kept"
    shift
  done
}

main "$@"
