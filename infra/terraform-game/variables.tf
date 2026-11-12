variable "region" {
  description = "The AWS region of the game machine: the one of infra/terraform/, which holds its network rules and its role."
  type        = string
}

variable "instance_type" {
  description = "The instance type: an x86 machine whose CPU is never slowed down, since the package of a release is x86; c7i.xlarge has 4 vCPU and 8 GiB."
  type        = string
  default     = "c7i.xlarge"
}

variable "github_repository" {
  description = "The repository the machine downloads the release and the scripts of infra/ from."
  type        = string
  default     = "nique-epi/R-Type"

  validation {
    condition     = can(regex("^[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+$", var.github_repository))
    error_message = "github_repository must be <owner>/<name>."
  }
}

variable "scripts_ref" {
  description = "The commit of the repository the machine downloads the scripts of infra/ at: the one the workflow runs on."
  type        = string

  validation {
    condition     = can(regex("^[0-9a-f]{40}$", var.scripts_ref))
    error_message = "scripts_ref must be a full commit id."
  }
}

variable "master_url" {
  description = "The url of the master the game servers are listed on, such as https://master.example.org."
  type        = string

  validation {
    condition     = can(regex("^https://([a-z0-9]([a-z0-9-]*[a-z0-9])?\\.)+[a-z]{2,}$", var.master_url))
    error_message = "master_url must be https://<domain>, with no port or path."
  }
}
