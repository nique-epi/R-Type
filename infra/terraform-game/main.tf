# The game machine of a LAN: the workflow lan-open applies this folder and
# lan-close destroys it, so the machine exists and is billed only while a LAN
# is open. Its network rules and its role are created once by
# infra/terraform/, and only read here.

terraform {
  required_version = ">= 1.10"

  required_providers {
    aws = {
      source  = "hashicorp/aws"
      version = "~> 6.0"
    }
  }

  # A state of its own: the workflows that apply and destroy it never read
  # the state of the master. The bucket and its region are given to
  # "terraform init" with -backend-config.
  backend "s3" {
    key          = "r-type/game.tfstate"
    use_lockfile = true
    encrypt      = true
  }
}

provider "aws" {
  region = var.region

  default_tags {
    tags = {
      project = "r-type"
      part    = "game"
    }
  }
}

data "aws_ssm_parameter" "ubuntu" {
  name = "/aws/service/canonical/ubuntu/server/24.04/stable/current/amd64/hvm/ebs-gp3/ami-id"
}

# Its tags hold the ports its rules open.
data "aws_security_group" "game" {
  name = "rtype-game"
}

locals {
  game_ports = range(
    data.aws_security_group.game.tags["first_game_port"],
    data.aws_security_group.game.tags["last_game_port"] + 1,
  )
  game_process_ports = join(" ", [
    data.aws_security_group.game.tags["first_game_process_port"],
    data.aws_security_group.game.tags["last_game_process_port"],
  ])
}

data "aws_iam_instance_profile" "game" {
  name = "rtype-game"
}

resource "aws_instance" "game" {
  ami                    = data.aws_ssm_parameter.ubuntu.value
  instance_type          = var.instance_type
  iam_instance_profile   = data.aws_iam_instance_profile.game.name
  vpc_security_group_ids = [data.aws_security_group.game.id]

  # The settings come first, as shell variables, then the script that reads
  # them.
  user_data = join("\n", [
    "#!/bin/sh",
    "REGION='${var.region}'",
    "REPOSITORY='${var.github_repository}'",
    "SCRIPTS_REF='${var.scripts_ref}'",
    "MASTER_URL='${var.master_url}'",
    "PORTS='${join(" ", local.game_ports)}'",
    "GAME_PROCESS_PORTS='${local.game_process_ports}'",
    file("${path.module}/user-data.sh"),
  ])

  metadata_options {
    http_tokens = "required"
  }

  root_block_device {
    volume_size = 8
    volume_type = "gp3"
    encrypted   = true
  }

  tags = {
    Name = "rtype-game"
  }

  # The script of the first boot names the commit of the workflow, and the
  # image is the latest of Ubuntu: both change between two runs. A machine
  # that exists is left as it is, since its script runs only once and a new
  # one would stop it, ending its games; lan-close then lan-open makes a
  # machine with the new ones.
  lifecycle {
    ignore_changes = [ami, user_data]
  }
}
