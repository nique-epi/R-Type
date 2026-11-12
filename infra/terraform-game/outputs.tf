output "server_count" {
  description = "How many game servers the machine starts: one per port of the pool."
  value       = length(local.game_ports)
}

output "public_ip" {
  description = "The address the players and the master reach the game servers at."
  value       = aws_instance.game.public_ip
}
