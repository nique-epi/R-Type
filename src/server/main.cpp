#include "NetworkContext.hpp"
#include "World.hpp"

int main() {
  rtype::game::World world;
  world.spawnPlayer();
  rtype::network::NetworkContext network;
  network.run();
  return 0;
}
