#!/bin/sh
# Prepares the machine of the master once, at its first boot. The service
# itself is installed by infra/master/deploy.sh.
set -eu

useradd --system --home-dir /var/lib/rtype --shell /usr/sbin/nologin rtype
install -d -o rtype -g rtype /var/lib/rtype
install -d /opt/rtype/releases

fallocate -l 1G /swapfile
chmod 600 /swapfile
mkswap /swapfile
swapon /swapfile
echo '/swapfile none swap sw 0 0' >> /etc/fstab
