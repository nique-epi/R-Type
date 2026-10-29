#include <exception>
#include <iostream>
#include <string>
#include "Application.hpp"
#include "ClientConstants.hpp"
#include "ClientModules.hpp"
#include "FrameLoop.hpp"
#include "LaunchArguments.hpp"
#include "LaunchOptions.hpp"
#include "LogLaunchOptions.hpp"
#include "Logger.hpp"
#include "LoggingConstants.hpp"
#include "LoopStages.hpp"
#include "SystemClock.hpp"

namespace {

/**
 * @brief Starts the modules of the client, runs its frame loop until the
 * window is closed or no screen is left, then stops the modules in the
 * reverse order: the connection leaves the server, then the window closes.
 * @returns the exit code: 0 once the client stopped, 1 after an error, which
 *          is logged and printed. No exception leaves it.
 */
int runClient(const rtype::logging::LogLaunchOptions& logOptions,
              const rtype::client::LaunchOptions& launchOptions) {
  try {
    const rtype::engine::SystemClock clock;
    rtype::client::FrameLoop loop{clock};
    rtype::engine::Application application{
        rtype::engine::LoopStages{rtype::client::CLIENT_LOOP_STAGES},
        rtype::client::clientModules(logOptions, launchOptions, loop)};
    application.start();
    loop.run(application);
    application.stop();
    const rtype::logging::Logger logger{
        std::string{rtype::client::CLIENT_LOGGER_NAME}};
    logger.info("client stopped");
  } catch (const std::exception& error) {
    const rtype::logging::Logger logger{
        std::string{rtype::client::CLIENT_LOGGER_NAME}};
    logger.error("client stopped: ", error.what());
    std::cerr << "client stopped: " << error.what() << '\n';
    return 1;
  }
  return 0;
}

}  // namespace

int main(int argumentCount, char** arguments) {
  try {
    const rtype::logging::LogLaunchOptions logOptions =
        rtype::logging::LogLaunchOptions::readFromProcess(
            argumentCount, arguments, rtype::logging::CLIENT_LOG_FILE_NAME);
    const rtype::client::LaunchOptions launchOptions =
        rtype::client::LaunchOptions::read(logOptions.otherArguments());
    if (launchOptions.helpRequested()) {
      std::cout << rtype::client::LaunchOptions::usage();
      return 0;
    }
    return runClient(logOptions, launchOptions);
  } catch (const std::exception& error) {
    std::cerr << rtype::engine::launchErrorText(rtype::client::PROGRAM_NAME,
                                                error.what());
    return 1;
  }
}
