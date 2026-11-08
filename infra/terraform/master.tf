data "aws_ssm_parameter" "ubuntu" {
  name = "/aws/service/canonical/ubuntu/server/24.04/stable/current/amd64/hvm/ebs-gp3/ami-id"
}

data "aws_iam_policy_document" "instance_trust" {
  statement {
    actions = ["sts:AssumeRole"]

    principals {
      type        = "Service"
      identifiers = ["ec2.amazonaws.com"]
    }
  }
}

resource "aws_iam_role" "instance" {
  name_prefix        = "rtype-master-instance-"
  assume_role_policy = data.aws_iam_policy_document.instance_trust.json
}

resource "aws_iam_role_policy_attachment" "ssm" {
  role       = aws_iam_role.instance.name
  policy_arn = "arn:aws:iam::aws:policy/AmazonSSMManagedInstanceCore"
}

resource "aws_iam_instance_profile" "master" {
  name_prefix = "rtype-master-"
  role        = aws_iam_role.instance.name
}

resource "aws_instance" "master" {
  ami                    = data.aws_ssm_parameter.ubuntu.value
  instance_type          = var.instance_type
  iam_instance_profile   = aws_iam_instance_profile.master.name
  vpc_security_group_ids = [aws_security_group.master.id]
  user_data              = file("${path.module}/user-data.sh")

  metadata_options {
    http_tokens = "required"
  }

  # A t3 machine runs in "unlimited" mode by default and bills the CPU it uses
  # past its credits. "standard" slows the CPU down instead, so the bill is the
  # price of the instance and nothing more.
  credit_specification {
    cpu_credits = "standard"
  }

  root_block_device {
    volume_size = 8
    volume_type = "gp3"
    encrypted   = true
  }

  tags = {
    Name = "rtype-master"
  }

  lifecycle {
    ignore_changes = [ami, user_data]
  }
}
