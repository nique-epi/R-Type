#include <exception>
#include <iostream>
#include "LogLaunchOptions.hpp"
#include "Logger.hpp"
#include "LoggingConstants.hpp"
#include "NetworkContext.hpp"
#include "World.hpp"

int main(int argumentCount, char** arguments) {
  try {
    rtype::logging::LogLaunchOptions::applyFromProcess(
        argumentCount, arguments, rtype::logging::SERVER_LOG_FILE_NAME);
    const rtype::logging::Logger logger{"Server"};
    logger.info("server starting");
    rtype::game::World world;
    world.spawnPlayer();
    rtype::network::NetworkContext network;
    network.run();
    logger.info("server stopped");
  } catch (const std::exception& error) {
    const rtype::logging::Logger logger{"Server"};
    logger.error("server stopped: ", error.what());
    std::cerr << "server stopped: " << error.what() << '\n';
    return 1;
  }
  return 0;
}
