variable "region" {
  description = "The AWS region of the master."
  type        = string
  default     = "eu-west-1"
}

variable "instance_type" {
  description = "The instance type: an x86 machine, since the package of a release is x86; t3.medium has 2 vCPU and 4 GiB."
  type        = string
  default     = "t3.medium"
}

variable "github_repository" {
  description = "The repository whose deployments may reach the master."
  type        = string
  default     = "nique-epi/R-Type"
}

variable "master_port" {
  description = "The TCP port the master listens on behind Caddy, reached from the machine only."
  type        = number
  default     = 8080
}

variable "master_domain" {
  description = "The domain name of the master, such as master.example.org: its A record points at the Elastic IP, and Caddy gets its certificate from Let's Encrypt for it."
  type        = string

  validation {
    condition     = can(regex("^([a-z0-9]([a-z0-9-]*[a-z0-9])?\\.)+[a-z]{2,}$", var.master_domain))
    error_message = "master_domain must be a domain name in lowercase, such as master.example.org."
  }
}

variable "state_bucket" {
  description = "The S3 bucket of the Terraform state: the workflows that open and close a LAN keep the state of the game machine in it."
  type        = string
}

variable "first_game_port" {
  description = "The port of the first game server of the pool of the game machine, UDP for the players and TCP for the content."
  type        = number
  default     = 4242
}

variable "last_game_port" {
  description = "The port of the last game server of the pool: 4247 makes a pool of 6."
  type        = number
  default     = 4247
}
