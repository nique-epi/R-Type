#include <exception>
#include <iostream>
#include "AssetFolder.hpp"
#include "AssetIds.hpp"
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "AssetLoadingStep.hpp"
#include "GameWindow.hpp"
#include "LogLaunchOptions.hpp"
#include "Logger.hpp"
#include "LoggingConstants.hpp"
#include "WindowConstants.hpp"

int main(int argumentCount, char** arguments) {
  try {
    rtype::logging::LogLaunchOptions::applyFromProcess(
        argumentCount, arguments, rtype::logging::CLIENT_LOG_FILE_NAME);
    const rtype::logging::Logger logger{"Client"};
    logger.info("client starting");
    rtype::client::AssetLibrary assets{rtype::client::locateAssetFolder()};
    logger.info("assets folder: ", rtype::client::genericText(assets.folder()));
    rtype::client::AssetLoadingStep startupLoading(
        assets, rtype::client::AssetList{
                    .fonts = {rtype::client::WINDOW_SIZE_LABEL_FONT_ID}});
    while (!startupLoading.isFinished()) {
      startupLoading.loadNext();
    }
    rtype::client::GameWindow gameWindow(
        assets.font(rtype::client::WINDOW_SIZE_LABEL_FONT_ID));
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
