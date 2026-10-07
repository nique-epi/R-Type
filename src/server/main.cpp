#include <cstddef>
#include <exception>
#include <iostream>
#include <span>
#include <string>
#include "Endpoint.hpp"
#include "LogLaunchOptions.hpp"
#include "Logger.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConstants.hpp"
#include "NetworkContext.hpp"
#include "UdpSocket.hpp"
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
    rtype::network::UdpSocket socket{
        network, rtype::network::Endpoint{
                     .address = std::string{rtype::network::ANY_IPV4_ADDRESS},
                     .port = rtype::network::DEFAULT_SERVER_PORT}};
    socket.startReceiving([&logger](const rtype::network::Endpoint& sender,
                                    std::span<const std::byte> payload) {
      logger.debug("received ", payload.size(), " bytes from ", sender.address,
                   " port ", sender.port);
    });
    logger.info("listening on UDP port ", socket.localEndpoint().port);
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
