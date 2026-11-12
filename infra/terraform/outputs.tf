output "master_url" {
  description = "The url of the master, for master-url on the game servers and the client, MASTER_URL on the web site and the deployment workflow."
  value       = "https://${var.master_domain}"
}

output "master_public_ip" {
  description = "The address the A record of master_domain points at."
  value       = aws_eip.master.public_ip
}

output "instance_id" {
  description = "The instance the deployments target."
  value       = aws_instance.master.id
}

output "deploy_role_arn" {
  description = "The role the deployment workflow assumes."
  value       = aws_iam_role.deploy.arn
}

output "lan_role_arn" {
  description = "The role the workflows that open and close a LAN assume."
  value       = aws_iam_role.lan.arn
}
