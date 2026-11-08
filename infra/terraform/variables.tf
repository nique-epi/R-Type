variable "region" {
  description = "The AWS region of the master."
  type        = string
  default     = "eu-west-1"
}

variable "instance_type" {
  description = "The instance type; t3.nano is the cheapest x86 one."
  type        = string
  default     = "t3.nano"
}

variable "github_repository" {
  description = "The repository whose deployments may reach the master."
  type        = string
  default     = "nique-epi/R-Type"
}

variable "master_port" {
  description = "The TCP port of the HTTP API of the master."
  type        = number
  default     = 8080
}
