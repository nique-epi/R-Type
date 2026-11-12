output "public_ip" {
  description = "The address the players and the master reach the game servers at."
  value       = aws_instance.game.public_ip
}
