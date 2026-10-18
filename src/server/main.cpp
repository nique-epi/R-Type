#include <cstdint>
#include <exception>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectionAcceptor.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "LaunchArguments.hpp"
#include "LaunchOptions.hpp"
#include "LogLaunchOptions.hpp"
#include "Logger.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "ServerApplication.hpp"
#include "ServerConstants.hpp"
#include "ShipSpawnAnnouncer.hpp"
#include "SystemClock.hpp"
#include "World.hpp"
#include "WorldStateSender.hpp"

namespace {

/**
 * @brief Applies the logging options, then runs the server on the port of
 * @p launchOptions until it stops.
 * @returns the exit code: 0 once the server stopped, 1 after an error, which
 *          is logged and printed. No exception leaves it.
 */
int runServer(const rtype::logging::LogLaunchOptions& logOptions,
              const rtype::server::LaunchOptions& launchOptions) {
  try {
    logOptions.apply();
    const rtype::logging::Logger logger{
        std::string{rtype::server::SERVER_LOGGER_NAME}};
    logger.info("server starting");
    rtype::game::World world;
    const std::vector<rtype::network::Endpoint> recipients;
    std::uint32_t tick = 0;
    rtype::server::ServerApplication* serverPointer = nullptr;
    const auto queueDatagram =
        [&serverPointer](rtype::network::OutgoingDatagram datagram) {
          serverPointer->queueDatagram(std::move(datagram));
        };
    rtype::server::WorldStateSender worldStateSender{queueDatagram};
    rtype::server::PlayerRoster roster;
    rtype::server::ShipSpawnAnnouncer shipSpawnAnnouncer{world, roster,
                                                         queueDatagram};
    rtype::server::ConnectionAcceptor connectionAcceptor{
        roster, queueDatagram,
        [&serverPointer](const rtype::network::Endpoint& player) {
          serverPointer->admit(player);
        },
        [&shipSpawnAnnouncer](rtype::server::ConnectedPlayer& player) {
          shipSpawnAnnouncer.onPlayerJoined(player);
        }};
    const rtype::engine::SystemClock clock;
    rtype::server::ServerApplication server{
        rtype::network::Endpoint{
            .address = std::string{rtype::network::ANY_IPV4_ADDRESS},
            .port = launchOptions.port()},
        [&logger, &world, &worldStateSender, &connectionAcceptor, &clock,
         &recipients, &tick](
            const rtype::server::ServerApplication::ReceivedDatagrams&
                received) {
          for (const rtype::network::IncomingDatagram& datagram :
               received.players) {
            logger.debug("received ", datagram.payload.size(), " bytes from ",
                         datagram.sender.address, " port ",
                         datagram.sender.port);
          }
          connectionAcceptor.receive(received.connectionRequests, clock.now());
          worldStateSender.onTick(++tick, world.components(), recipients);
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

}  // namespace

int main(int argumentCount, char** arguments) {
  try {
    const rtype::logging::LogLaunchOptions logOptions =
        rtype::logging::LogLaunchOptions::readFromProcess(
            argumentCount, arguments, rtype::logging::SERVER_LOG_FILE_NAME);
    const rtype::server::LaunchOptions launchOptions =
        rtype::server::LaunchOptions::read(logOptions.otherArguments());
    if (launchOptions.helpRequested()) {
      std::cout << rtype::server::LaunchOptions::usage();
      return 0;
    }
    return runServer(logOptions, launchOptions);
  } catch (const std::exception& error) {
    std::cerr << rtype::engine::launchErrorText(rtype::server::PROGRAM_NAME,
                                                error.what());
    return 1;
  }
}
