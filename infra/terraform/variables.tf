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

variable "github_subject_prefix" {
  description = "What the sub claim of the GitHub tokens of the repository whose deployments may reach the master starts with: repo:<owner>/<repo>, or repo:<owner>@<id>/<repo>@<id> when the repository uses immutable subjects. gh api repos/<owner>/<repo>/actions/oidc/customization/sub prints it as sub_claim_prefix."
  type        = string
  default     = "repo:nique-epi@260720952/R-Type@1392224722"

  validation {
    condition     = can(regex("^repo:[^:]+$", var.github_subject_prefix))
    error_message = "github_subject_prefix must be repo:<owner>/<repo>, with the ids of immutable subjects when the repository uses them."
  }
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
