#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "Endpoint.hpp"
#include "GameListSettings.hpp"
#include "GamesModule.hpp"
#include "IModule.hpp"
#include "LaunchArguments.hpp"
#include "LaunchOptions.hpp"
#include "LobbyModule.hpp"
#include "LogLaunchOptions.hpp"
#include "Logger.hpp"
#include "LoggingConstants.hpp"
#include "LoggingModule.hpp"
#include "LoopStages.hpp"
#include "NetworkConstants.hpp"
#include "NetworkModule.hpp"
#include "ServerConstants.hpp"

namespace {

/**
 * @returns The address and port the server listens on: every IPv4 interface,
 * on the port of @p launchOptions.
 */
rtype::network::Endpoint listeningEndpoint(
    const rtype::server::LaunchOptions& launchOptions) {
  return rtype::network::Endpoint{
      .address = std::string{rtype::network::ANY_IPV4_ADDRESS},
      .port = launchOptions.port()};
}

/**
 * @returns The modules of the server, in the order they install and start:
 * the journal, @p network, the games, then the lobby that seats the new
 * players in them.
 */
std::vector<std::unique_ptr<rtype::engine::IModule>> serverModules(
    const rtype::logging::LogLaunchOptions& logOptions,
    const rtype::server::LaunchOptions& launchOptions,
    std::unique_ptr<rtype::server::NetworkModule> network) {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<rtype::server::LoggingModule>(logOptions));
  modules.push_back(std::move(network));
  modules.push_back(std::make_unique<rtype::server::GamesModule>(
      rtype::server::GameListSettings{
          .seed = launchOptions.seed(),
          .waveFile = std::filesystem::path{rtype::server::WAVE_FILE_PATH}}));
  modules.push_back(std::make_unique<rtype::server::LobbyModule>());
  return modules;
}

/**
 * @brief Starts the modules of the server, runs it until a stop signal, then
 * stops the modules in the reverse order: the players are told the server
 * stops, then the socket closes.
 * @returns the exit code: 0 once the server stopped, 1 after an error, which
 *          is logged and printed. No exception leaves it.
 */
int runServer(const rtype::logging::LogLaunchOptions& logOptions,
              const rtype::server::LaunchOptions& launchOptions) {
  try {
    auto network = std::make_unique<rtype::server::NetworkModule>(
        listeningEndpoint(launchOptions));
    rtype::server::NetworkModule& pacer = *network;
    rtype::engine::Application application{
        rtype::engine::LoopStages(rtype::server::SERVER_LOOP_STAGES),
        serverModules(logOptions, launchOptions, std::move(network))};
    application.start();
    pacer.run();
    application.stop();
    const rtype::logging::Logger logger{
        std::string{rtype::server::SERVER_LOGGER_NAME}};
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
