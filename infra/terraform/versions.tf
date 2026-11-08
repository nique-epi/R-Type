terraform {
  required_version = ">= 1.10"

  required_providers {
    aws = {
      source  = "hashicorp/aws"
      version = "~> 6.0"
    }
  }

  # The bucket and its region are given to "terraform init" with
  # -backend-config, since a bucket name is unique in the world.
  backend "s3" {
    key          = "r-type/master.tfstate"
    use_lockfile = true
    encrypt      = true
  }
}

provider "aws" {
  region = var.region

  default_tags {
    tags = {
      project = "r-type"
      part    = "master"
    }
  }
}
