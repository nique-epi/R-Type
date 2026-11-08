data "aws_vpc" "default" {
  default = true
}

resource "aws_security_group" "master" {
  name_prefix = "rtype-master-"
  description = "HTTP API of the R-Type master"
  vpc_id      = data.aws_vpc.default.id
}

# Caddy answers HTTPS on 443 and forwards to the master on the machine; the
# port of the master itself is open to nobody. Port 80 serves the challenges
# of Let's Encrypt and sends any other request to HTTPS.
resource "aws_vpc_security_group_ingress_rule" "https" {
  security_group_id = aws_security_group.master.id
  description       = "HTTPS API of the master, through Caddy"
  ip_protocol       = "tcp"
  from_port         = 443
  to_port           = 443
  cidr_ipv4         = "0.0.0.0/0"
}

resource "aws_vpc_security_group_ingress_rule" "http" {
  security_group_id = aws_security_group.master.id
  description       = "Certificate challenges and the redirect to HTTPS"
  ip_protocol       = "tcp"
  from_port         = 80
  to_port           = 80
  cidr_ipv4         = "0.0.0.0/0"
}

resource "aws_vpc_security_group_egress_rule" "all" {
  security_group_id = aws_security_group.master.id
  description       = "Downloads, probes of the game servers, SSM"
  ip_protocol       = "-1"
  cidr_ipv4         = "0.0.0.0/0"
}

resource "aws_eip" "master" {
  domain   = "vpc"
  instance = aws_instance.master.id

  # The game servers, the client and the web site all hold this address: a
  # destroy that gave a new one would break them.
  lifecycle {
    prevent_destroy = true
  }
}
