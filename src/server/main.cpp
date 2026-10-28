#include <exception>
#include <iostream>
#include <string>
#include "Application.hpp"
#include "LaunchArguments.hpp"
#include "LaunchOptions.hpp"
#include "LogLaunchOptions.hpp"
#include "Logger.hpp"
#include "LoggingConstants.hpp"
#include "LoopStages.hpp"
#include "ServerConstants.hpp"
#include "ServerModules.hpp"
#include "StopReason.hpp"
#include "StopRequest.hpp"
#include "StopSignals.hpp"

namespace {

/**
 * @brief Tells the operator and the journal that the server stopped on an
 * error.
 */
void reportFailure(const std::string& text) {
  const rtype::logging::Logger logger{
      std::string{rtype::server::SERVER_LOGGER_NAME}};
  logger.error("server stopped: ", text);
  std::cerr << "server stopped: " << text << '\n';
}

/**
 * @brief Runs the modules of the server until Ctrl-C, SIGTERM or a failure
 * asks it to stop, then stops them.
 * @returns the exit code: 0 once the server stopped when asked, 1 after an
 *          error, which is logged and printed. No exception leaves it.
 */
int runServer(const rtype::logging::LogLaunchOptions& logOptions,
              const rtype::server::LaunchOptions& launchOptions) {
  rtype::server::StopRequest stopRequest;
  const rtype::server::StopSignals stopSignals{stopRequest};
  int exitCode = 0;
  try {
    rtype::engine::Application application{
        rtype::engine::LoopStages{rtype::server::SERVER_LOOP_STAGES},
        rtype::server::serverModules(logOptions, launchOptions, stopRequest)};
    application.start();
    const rtype::server::StopReason reason = stopRequest.wait();
    application.stop();
    if (reason.failed) {
      reportFailure(reason.text);
      exitCode = 1;
    }
  } catch (const std::exception& error) {
    reportFailure(error.what());
    exitCode = 1;
  }
  stopRequest.announceStopped();
  return exitCode;
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
