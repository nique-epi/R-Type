#include <exception>
#include <iostream>
#include "GameWindow.hpp"
#include "LogLaunchOptions.hpp"
#include "Logger.hpp"
#include "LoggingConstants.hpp"

int main(int argumentCount, char** arguments) {
  try {
    rtype::logging::LogLaunchOptions::applyFromProcess(
        argumentCount, arguments, rtype::logging::CLIENT_LOG_FILE_NAME);
    const rtype::logging::Logger logger{"Client"};
    logger.info("client starting");
    rtype::client::GameWindow gameWindow;
    gameWindow.run();
    logger.info("client stopped");
  } catch (const std::exception& error) {
    const rtype::logging::Logger logger{"Client"};
    logger.error("client stopped: ", error.what());
    std::cerr << "client stopped: " << error.what() << '\n';
    return 1;
  }
  return 0;
}
