#!/bin/sh
# Prints "<port> <status> <games> <most games>" for each game server the
# master lists at an address: the games it runs and the games it can run.
# Fails when the master does not answer with a listing. The workflows that
# open and close a LAN read the master through it.
# Usage: listed-servers.sh <url of the master> <address>
set -eu

answer=$(curl --fail --silent --show-error --max-time 5 "$1/api/servers?limit=100")
printf '%s' "$answer" | jq -r --arg address "$2" '
  .servers[] | select(.address == $address) |
  "\(.udpPort) \(.status) \(.gameCount) \(.maximumGames)"'
