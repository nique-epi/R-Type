#include <exception>
#include <iostream>
#include <span>
#include <string>
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "LaunchArguments.hpp"
#include "LaunchOptions.hpp"
#include "LogLaunchOptions.hpp"
#include "Logger.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConstants.hpp"
#include "ServerApplication.hpp"
#include "ServerConstants.hpp"
#include "World.hpp"

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
    world.spawnPlayer();
    rtype::server::ServerApplication server{
        rtype::network::Endpoint{
            .address = std::string{rtype::network::ANY_IPV4_ADDRESS},
            .port = launchOptions.port()},
        [&logger](std::span<const rtype::network::IncomingDatagram> datagrams) {
          for (const rtype::network::IncomingDatagram& datagram : datagrams) {
            logger.debug("received ", datagram.payload.size(), " bytes from ",
                         datagram.sender.address, " port ",
                         datagram.sender.port);
          }
        }};
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
