#include <exception>
#include <iostream>
#include <string>
#include "Application.hpp"
#include "AssetFolder.hpp"
#include "AssetIds.hpp"
#include "AssetLibrary.hpp"
#include "ClientConstants.hpp"
#include "LaunchArguments.hpp"
#include "LaunchOptions.hpp"
#include "LogLaunchOptions.hpp"
#include "Logger.hpp"
#include "LoggingConstants.hpp"
#include "ScreenAssets.hpp"

namespace {

/**
 * @brief Applies the logging options, logs the server given at launch, then
 * runs the client until its window closes.
 * @returns the exit code: 0 once the window closed, 1 after an error, which
 *          is logged and printed. No exception leaves it.
 */
int runClient(const rtype::logging::LogLaunchOptions& logOptions,
              const rtype::client::LaunchOptions& launchOptions) {
  try {
    logOptions.apply();
    const rtype::logging::Logger logger{
        std::string{rtype::client::CLIENT_LOGGER_NAME}};
    logger.info("client starting");
    logger.info("server: ", launchOptions.serverEndpoint().address, " port ",
                launchOptions.serverEndpoint().port);
    rtype::client::AssetLibrary assets{rtype::client::locateAssetFolder()};
    logger.info("assets folder: ", rtype::client::genericText(assets.folder()));
    rtype::client::loadInterfaceAssets(assets);
    rtype::client::Application application{assets};
    application.run();
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
