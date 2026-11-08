#!/bin/sh
# Installs the server package of a release on the master instance and
# restarts the master on it; goes back to the previous release when the new
# one does not answer. The workflow runs it through SSM Run Command, after a
# first line VERSION=<x.y.z>.
set -eu

: "${VERSION:?VERSION is not set}"
case $VERSION in
  '' | *[!0-9.]*)
    echo "invalid version: $VERSION" >&2
    exit 1
    ;;
esac

repository=nique-epi/R-Type
port=8080
releases=/opt/rtype/releases
current=/opt/rtype/current
name=R-Type-server-$VERSION-Linux
url=https://github.com/$repository/releases/download/v$VERSION/$name.tar.gz

previous=$(readlink "$current" 2>/dev/null || true)

# Download and unpack first: a missing release stops here, the service untouched.
if [ ! -d "$releases/$name" ]; then
  work=$(mktemp -d)
  trap 'rm -rf "$work"' EXIT
  curl --fail --silent --show-error --location "$url" --output "$work/server.tar.gz"
  tar -xzf "$work/server.tar.gz" -C "$work"
  mv "$work/$name" "$releases/$name"
fi

cat > /etc/systemd/system/rtype-master.service <<UNIT
[Unit]
Description=R-Type master
After=network-online.target
Wants=network-online.target

[Service]
User=rtype
Group=rtype
WorkingDirectory=/var/lib/rtype
ExecStart=$current/bin/r-type_server --mode master -p $port --database /var/lib/rtype/r-type_master.db --no-log-file
Restart=on-failure
RestartSec=5
NoNewPrivileges=true
ProtectSystem=strict
ReadWritePaths=/var/lib/rtype
PrivateTmp=true

[Install]
WantedBy=multi-user.target
UNIT

systemctl daemon-reload
systemctl enable rtype-master

ln -sfn "$releases/$name" "$current"
systemctl restart rtype-master

healthy=0
for _ in $(seq 1 20); do
  if curl --fail --silent "http://127.0.0.1:$port/api/health" > /dev/null; then
    healthy=1
    break
  fi
  sleep 1
done

if [ "$healthy" -ne 1 ]; then
  echo "the master of $VERSION does not answer" >&2
  journalctl -u rtype-master --no-pager -n 30 >&2 || true
  if [ -n "$previous" ]; then
    ln -sfn "$previous" "$current"
    systemctl restart rtype-master
    echo "went back to $previous" >&2
  fi
  exit 1
fi

# Keep the release in use and the previous one.
for release in "$releases"/*/; do
  release=${release%/}
  if [ "$release" != "$releases/$name" ] && [ "$release" != "$previous" ]; then
    rm -rf "$release"
  fi
done

echo "the master runs $VERSION"
