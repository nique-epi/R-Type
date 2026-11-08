output "master_url" {
  description = "The url of the master, for master-url on the game servers and MASTER_URL on the web site."
  value       = "http://${aws_eip.master.public_ip}:${var.master_port}"
}

output "instance_id" {
  description = "The instance the deployments target."
  value       = aws_instance.master.id
}

output "deploy_role_arn" {
  description = "The role the deployment workflow assumes."
  value       = aws_iam_role.deploy.arn
}
