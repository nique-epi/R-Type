#!/bin/sh
# What the scripts that install a release share: the check of its version,
# its download, and the systemd units of a server. infra/game-servers/install.sh
# reads this file; the deployment workflow sends it to the master instance in
# front of infra/master/deploy.sh.

# The repository the releases come from; REPOSITORY names another one.
repository=${REPOSITORY:-nique-epi/R-Type}

# The version comes from a tag: it is never used before it is checked.
require_version() {
  : "${VERSION:?VERSION is not set}"
  case $VERSION in
    '' | *[!0-9.]*)
      echo "invalid version: $VERSION" >&2
      exit 1
      ;;
  esac
}

# Downloads the server package $2 of the release VERSION and unpacks it in the
# folder $1, next to the others. A missing release stops here, before
# anything changed on the machine.
unpack_release() {
  releases=$1
  name=$2
  if [ -d "$releases/$name" ]; then
    return 0
  fi
  url=https://github.com/$repository/releases/download/v$VERSION/$name.tar.gz
  work=$(mktemp -d)
  trap 'rm -rf "$work"' EXIT
  curl --fail --silent --show-error --location "$url" --output "$work/server.tar.gz"
  tar -xzf "$work/server.tar.gz" -C "$work"
  mv "$work/$name" "$releases/$name"
}

# Gives the server of $2 a copy of the release folder $1 that it owns. A
# supervisor writes the commands and the journals of its games next to its
# executable, under names two supervisors share, so each one runs from its
# own copy.
copy_release_for() {
  rm -rf "$2/release"
  cp -a "$1" "$2/release"
  chown -R rtype:rtype "$2/release"
}

# Writes the service $1, described as $2, which runs the command $4 as the
# user rtype from the folder $3, the only one it may write in. systemd starts
# it again 5 seconds after it stopped by itself.
write_server_unit() {
  cat > "/etc/systemd/system/$1.service" <<UNIT
[Unit]
Description=$2
After=network-online.target
Wants=network-online.target

[Service]
User=rtype
Group=rtype
WorkingDirectory=$3
ExecStart=$4
Restart=on-failure
RestartSec=5
NoNewPrivileges=true
ProtectSystem=strict
ReadWritePaths=$3
PrivateTmp=true

[Install]
WantedBy=multi-user.target
UNIT
}

# Writes the timer $1, described as $2, which runs the service of its name $3
# seconds after the boot, then every $4 seconds.
write_timer() {
  cat > "/etc/systemd/system/$1.timer" <<UNIT
[Unit]
Description=$2

[Timer]
OnBootSec=$3
OnUnitActiveSec=$4
AccuracySec=1

[Install]
WantedBy=timers.target
UNIT
}
