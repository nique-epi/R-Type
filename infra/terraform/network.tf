data "aws_vpc" "default" {
  default = true
}

resource "aws_security_group" "master" {
  name_prefix = "rtype-master-"
  description = "HTTP API of the R-Type master"
  vpc_id      = data.aws_vpc.default.id
}

resource "aws_vpc_security_group_ingress_rule" "api" {
  security_group_id = aws_security_group.master.id
  description       = "HTTP API of the master"
  ip_protocol       = "tcp"
  from_port         = var.master_port
  to_port           = var.master_port
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
