#include <exception>
#include <iostream>
#include <string>
#include "Endpoint.hpp"
#include "LogLaunchOptions.hpp"
#include "Logger.hpp"
#include "LoggingConstants.hpp"
#include "NetworkConstants.hpp"
#include "ServerApplication.hpp"
#include "ServerConstants.hpp"

int main(int argumentCount, char** arguments) {
  try {
    rtype::logging::LogLaunchOptions::applyFromProcess(
        argumentCount, arguments, rtype::logging::SERVER_LOG_FILE_NAME);
    const rtype::logging::Logger logger{
        std::string{rtype::server::SERVER_LOGGER_NAME}};
    logger.info("server starting");
    rtype::server::ServerApplication server{rtype::network::Endpoint{
        .address = std::string{rtype::network::ANY_IPV4_ADDRESS},
        .port = rtype::network::DEFAULT_SERVER_PORT}};
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
