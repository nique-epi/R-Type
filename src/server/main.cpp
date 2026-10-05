#include <exception>
#include <iostream>
#include "NetworkContext.hpp"
#include "World.hpp"

int main() {
  try {
    rtype::game::World world;
    world.spawnPlayer();
    rtype::network::NetworkContext network;
    network.run();
  } catch (const std::exception& error) {
    std::cerr << "server stopped: " << error.what() << '\n';
    return 1;
  }
  return 0;
}
