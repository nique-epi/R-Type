#!/bin/sh
# Prepares the game machine of a LAN at its first boot with what only this
# machine knows, then hands over to infra/game-servers/install.sh on the
# latest release: the keys of the master read from Parameter Store, its
# public address, one server.cfg per server and the settings of the
# autoscaler. Terraform writes the variables REGION, REPOSITORY, SCRIPTS_REF,
# MASTER_URL, PORTS and GAME_PROCESS_PORTS in front of this script.
set -eu

servers=/var/lib/rtype-servers
scripts=/opt/rtype-servers/infra

# A game listens on a port the system picks: this keeps them in the range
# the network rules open.
narrow_the_game_ports() {
  echo "net.ipv4.ip_local_port_range = $GAME_PROCESS_PORTS" > /etc/sysctl.d/60-rtype-game-ports.conf
  sysctl --system > /dev/null
}

# Prints the public address of this machine, which the metadata service of
# AWS gives.
public_address() {
  token=$(curl --fail --silent --request PUT http://169.254.169.254/latest/api/token \
    --header 'X-aws-ec2-metadata-token-ttl-seconds: 60')
  curl --fail --silent --header "X-aws-ec2-metadata-token: $token" \
    http://169.254.169.254/latest/meta-data/public-ipv4
}

# Writes the server.cfg of each server of the pool, with the key
# infra/game-servers/create-keys.sh kept for its port. Only root reads them
# here; install.sh gives them to the user of the servers.
write_server_settings() {
  snap install aws-cli --channel=v2/stable --classic
  umask 077
  number=0
  for port in $PORTS; do
    number=$((number + 1))
    key=$(/snap/bin/aws ssm get-parameter --region "$REGION" --with-decryption \
      --name "/rtype/game/$port/master-key" --query Parameter.Value --output text)
    install -d "$servers/$port"
    cat > "$servers/$port/server.cfg" <<CFG
set master-url $MASTER_URL
set master-key "$key"
set public-address $address
set server-name "LAN $number"
CFG
  done
  umask 022
}

write_autoscaler_settings() {
  cat > /etc/rtype-autoscaler.conf <<CONF
MASTER_URL=$MASTER_URL
PUBLIC_ADDRESS=$address
PORTS="$PORTS"
SCALE_OUT_PERCENT=70
CONF
}

# The scripts come from the commit the workflow ran on, in the folders they
# have in the repository, since install.sh reads release.sh from its parent.
download_scripts() {
  raw=https://raw.githubusercontent.com/$REPOSITORY/$SCRIPTS_REF/infra
  install -d "$scripts/game-servers"
  for script in release.sh game-servers/install.sh game-servers/autoscale.sh \
    game-servers/listed-servers.sh; do
    curl --fail --silent --show-error --location "$raw/$script" --output "$scripts/$script"
  done
}

# Prints the version of the latest release: the page of the latest release
# redirects to the page of its tag, v<version>.
latest_version() {
  page=$(curl --fail --silent --show-error --head --output /dev/null \
    --write-out '%{redirect_url}' "https://github.com/$REPOSITORY/releases/latest")
  echo "${page##*/v}"
}

main() {
  narrow_the_game_ports
  address=$(public_address)
  write_server_settings
  write_autoscaler_settings
  download_scripts
  REPOSITORY=$REPOSITORY VERSION=$(latest_version) sh "$scripts/game-servers/install.sh"
}

main
