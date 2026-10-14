#include <exception>
#include <iostream>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "LogLaunchOptions.hpp"
#include "Logger.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "ServerApplication.hpp"
#include "ServerConstants.hpp"
#include "World.hpp"
#include "WorldStateSender.hpp"

int main(int argumentCount, char** arguments) {
  try {
    rtype::logging::LogLaunchOptions::applyFromProcess(
        argumentCount, arguments, rtype::logging::SERVER_LOG_FILE_NAME);
    const rtype::logging::Logger logger{
        std::string{rtype::server::SERVER_LOGGER_NAME}};
    logger.info("server starting");
    rtype::game::World world;
    world.spawnPlayer();
    const std::vector<rtype::network::Endpoint> recipients;
    rtype::server::ServerApplication* serverPointer = nullptr;
    rtype::server::WorldStateSender worldStateSender{
        [&serverPointer](rtype::network::OutgoingDatagram datagram) {
          serverPointer->queueDatagram(std::move(datagram));
        }};
    rtype::server::ServerApplication server{
        rtype::network::Endpoint{
            .address = std::string{rtype::network::ANY_IPV4_ADDRESS},
            .port = rtype::network::DEFAULT_SERVER_PORT},
        [&logger, &world, &worldStateSender, &recipients](
            std::span<const rtype::network::IncomingDatagram> datagrams) {
          for (const rtype::network::IncomingDatagram& datagram : datagrams) {
            logger.debug("received ", datagram.payload.size(), " bytes from ",
                         datagram.sender.address, " port ",
                         datagram.sender.port);
          }
          worldStateSender.onTick(world.components(), recipients);
        }};
    serverPointer = &server;
    logger.info("listening on UDP port ", server.localEndpoint().port);
    server.run();
    logger.info("server stopped");
  } catch (const std::exception& error) {
    const rtype::logging::Logger logger{
        std::string{rtype::server::SERVER_LOGGER_NAME}};
    logger.error("server stopped: ", error.what());
    std::cerr << "server stopped: " << error.what() << '\n';
    return 1;
  }
  return 0;
}
