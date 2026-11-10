#!/bin/sh
#
# Signs the update feeds of the client with the key of the releases.
#
# Usage: UPDATE_SIGNING_KEY=<Ed25519 private key, PEM> \
#          sh packaging/sign-update-feeds.sh <folder holding the feeds>
#
# The release workflow runs it on the packages it publishes. For each feed vpk
# wrote, releases.win.json, releases.osx.json and releases.linux.json, it
# writes the Ed25519 signature next to it, as releases.<channel>.json.sig,
# which the client checks before Velopack reads the feed
# (src/client/Update/Feed/SignedFeedSource.cpp). It stops when a feed is
# missing, when no key is given, or when the public half of the key is not the
# UPDATE_PUBLIC_KEY the client holds (src/client/Update/Feed/FeedConstants.hpp):
# a release signed with another key would never reach a player.

set -eu

feeds_folder=$1
repository=$(cd "$(dirname "$0")/.." && pwd)
constants=$repository/src/client/Update/Feed/FeedConstants.hpp

if [ -z "${UPDATE_SIGNING_KEY:-}" ]; then
  echo "FAILED: the secret UPDATE_SIGNING_KEY is not set"
  exit 1
fi

work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
key=$work/key.pem
(umask 077 && printf '%s\n' "$UPDATE_SIGNING_KEY" > "$key")

# The public half of the key, as the 64 hexadecimal digits of its raw bytes,
# and the one the client holds, read from the bytes of UPDATE_PUBLIC_KEY.
signing_key=$(openssl pkey -in "$key" -pubout -outform DER | tail -c 32 | od -An -v -tx1 | tr -d ' \n')
client_key=$(sed -n '/UPDATE_PUBLIC_KEY{/,/};/p' "$constants" | grep -o '0x[0-9a-f][0-9a-f]' | sed 's/0x//' | tr -d '\n')
if [ "$signing_key" != "$client_key" ]; then
  echo "FAILED: UPDATE_SIGNING_KEY is not the key whose public half the client holds"
  exit 1
fi

openssl pkey -in "$key" -pubout -out "$work/public.pem"
for channel in win osx linux; do
  feed=$feeds_folder/releases.$channel.json
  if [ ! -f "$feed" ]; then
    echo "FAILED: no feed $feed"
    exit 1
  fi
  openssl pkeyutl -sign -rawin -inkey "$key" -in "$feed" -out "$feed.sig"
  openssl pkeyutl -verify -rawin -pubin -inkey "$work/public.pem" \
    -in "$feed" -sigfile "$feed.sig" > /dev/null
  echo "OK: signed $feed"
done
