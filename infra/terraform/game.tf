# What the game machine of a LAN needs and that costs nothing while it does
# not exist: its network rules, its role, and the role of the workflows that
# create and destroy it. The machine itself is described in
# infra/terraform-game/, with a state of its own.

data "aws_caller_identity" "current" {}

locals {
  # The range the game machine gives its games: a game listens on a port the
  # system picks, which infra/terraform-game/user-data.sh narrows to it.
  first_game_process_port = 40000
  last_game_process_port  = 40999

  game_state_key = "r-type/game.tfstate"
}

resource "aws_security_group" "game" {
  name        = "rtype-game"
  description = "Game servers of the R-Type LAN"
  vpc_id      = data.aws_vpc.default.id

  # infra/terraform-game/ reads the ranges here, so the ports the machine
  # uses are the ports these rules open.
  tags = {
    part                    = "game"
    first_game_port         = var.first_game_port
    last_game_port          = var.last_game_port
    first_game_process_port = local.first_game_process_port
    last_game_process_port  = local.last_game_process_port
  }
}

resource "aws_vpc_security_group_ingress_rule" "game_supervisors" {
  security_group_id = aws_security_group.game.id
  description       = "Players reaching the servers of the pool"
  ip_protocol       = "udp"
  from_port         = var.first_game_port
  to_port           = var.last_game_port
  cidr_ipv4         = "0.0.0.0/0"
}

resource "aws_vpc_security_group_ingress_rule" "game_content" {
  security_group_id = aws_security_group.game.id
  description       = "Content packs of the servers of the pool"
  ip_protocol       = "tcp"
  from_port         = var.first_game_port
  to_port           = var.last_game_port
  cidr_ipv4         = "0.0.0.0/0"
}

resource "aws_vpc_security_group_ingress_rule" "game_processes" {
  security_group_id = aws_security_group.game.id
  description       = "Players reaching the games the servers start"
  ip_protocol       = "udp"
  from_port         = local.first_game_process_port
  to_port           = local.last_game_process_port
  cidr_ipv4         = "0.0.0.0/0"
}

resource "aws_vpc_security_group_egress_rule" "game_all" {
  security_group_id = aws_security_group.game.id
  description       = "Downloads, the master, SSM"
  ip_protocol       = "-1"
  cidr_ipv4         = "0.0.0.0/0"
}

resource "aws_iam_role" "game" {
  name               = "rtype-game"
  assume_role_policy = data.aws_iam_policy_document.instance_trust.json
}

resource "aws_iam_role_policy_attachment" "game_ssm" {
  role       = aws_iam_role.game.name
  policy_arn = "arn:aws:iam::aws:policy/AmazonSSMManagedInstanceCore"
}

# The keys of the master of the pool, which infra/game-servers/create-keys.sh
# keeps under /rtype/game/. They are encrypted with the default key of
# Parameter Store, which every principal of the account may decrypt with.
data "aws_iam_policy_document" "game_keys" {
  statement {
    actions   = ["ssm:GetParameter"]
    resources = ["arn:aws:ssm:${var.region}:${data.aws_caller_identity.current.account_id}:parameter/rtype/game/*"]
  }
}

resource "aws_iam_role_policy" "game_keys" {
  name   = "read-the-keys-of-the-master"
  role   = aws_iam_role.game.id
  policy = data.aws_iam_policy_document.game_keys.json
}

resource "aws_iam_instance_profile" "game" {
  name = "rtype-game"
  role = aws_iam_role.game.name
}

# The workflows lan-open and lan-close: they apply and destroy
# infra/terraform-game/, and stop and start the test server of the master
# machine. They read and write the state of the game machine only.
data "aws_iam_policy_document" "lan" {
  # Running a script on the master machine, as the deployments do.
  source_policy_documents = [data.aws_iam_policy_document.deploy.json]

  statement {
    sid       = "ListTheStateOfTheGameMachine"
    actions   = ["s3:ListBucket"]
    resources = ["arn:aws:s3:::${var.state_bucket}"]

    condition {
      test     = "StringLike"
      variable = "s3:prefix"
      values   = ["${local.game_state_key}*"]
    }
  }

  statement {
    sid     = "KeepTheStateOfTheGameMachine"
    actions = ["s3:GetObject", "s3:PutObject", "s3:DeleteObject"]
    resources = [
      "arn:aws:s3:::${var.state_bucket}/${local.game_state_key}",
      "arn:aws:s3:::${var.state_bucket}/${local.game_state_key}.tflock",
    ]
  }

  # Describe actions have no resource-level restriction.
  statement {
    sid       = "ReadWhatTheGameMachineUses"
    actions   = ["ec2:Describe*"]
    resources = ["*"]
  }

  statement {
    sid       = "ReadTheUbuntuImage"
    actions   = ["ssm:GetParameter"]
    resources = ["arn:aws:ssm:${var.region}::parameter/aws/service/canonical/*"]
  }

  statement {
    sid       = "ReadTheProfileOfTheGameMachine"
    actions   = ["iam:GetInstanceProfile"]
    resources = [aws_iam_instance_profile.game.arn]
  }

  statement {
    sid       = "CreateAnInstanceTaggedGame"
    actions   = ["ec2:RunInstances"]
    resources = ["arn:aws:ec2:${var.region}:${data.aws_caller_identity.current.account_id}:instance/*"]

    condition {
      test     = "StringEquals"
      variable = "aws:RequestTag/part"
      values   = ["game"]
    }
  }

  statement {
    sid     = "CreateItWithTheseParts"
    actions = ["ec2:RunInstances"]
    resources = [
      "arn:aws:ec2:${var.region}::image/*",
      "arn:aws:ec2:${var.region}:${data.aws_caller_identity.current.account_id}:volume/*",
      "arn:aws:ec2:${var.region}:${data.aws_caller_identity.current.account_id}:network-interface/*",
      "arn:aws:ec2:${var.region}:${data.aws_caller_identity.current.account_id}:subnet/*",
      aws_security_group.game.arn,
    ]
  }

  statement {
    sid       = "TagItWhenItIsCreated"
    actions   = ["ec2:CreateTags"]
    resources = ["arn:aws:ec2:${var.region}:${data.aws_caller_identity.current.account_id}:*/*"]

    condition {
      test     = "StringEquals"
      variable = "ec2:CreateAction"
      values   = ["RunInstances"]
    }
  }

  statement {
    sid       = "GiveItItsRole"
    actions   = ["iam:PassRole"]
    resources = [aws_iam_role.game.arn]
  }

  statement {
    sid       = "DestroyAnInstanceTaggedGame"
    actions   = ["ec2:TerminateInstances"]
    resources = ["arn:aws:ec2:${var.region}:${data.aws_caller_identity.current.account_id}:instance/*"]

    condition {
      test     = "StringEquals"
      variable = "aws:ResourceTag/part"
      values   = ["game"]
    }
  }
}

resource "aws_iam_role" "lan" {
  name               = "rtype-lan"
  assume_role_policy = data.aws_iam_policy_document.github_trust.json
}

resource "aws_iam_role_policy" "lan" {
  name   = "open-and-close-a-lan"
  role   = aws_iam_role.lan.id
  policy = data.aws_iam_policy_document.lan.json
}
