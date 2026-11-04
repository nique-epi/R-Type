#include <algorithm>
#include <exception>
#include <filesystem>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "ConfigurationLog.hpp"
#include "ConfigurationReader.hpp"
#include "ConfigurationReport.hpp"
#include "IModule.hpp"
#include "LaunchArguments.hpp"
#include "LaunchOptions.hpp"
#include "Logger.hpp"
#include "LoopStages.hpp"
#include "MasterConstants.hpp"
#include "MasterLaunchOptions.hpp"
#include "MasterModules.hpp"
#include "ServerConstants.hpp"
#include "ServerKeyOrder.hpp"
#include "ServerMode.hpp"
#include "ServerModeChoice.hpp"
#include "ServerModeOption.hpp"
#include "ServerModules.hpp"
#include "StopReason.hpp"
#include "StopRequest.hpp"
#include "StopSignals.hpp"

namespace {

/**
 * @brief What tells apart the two programs the binary runs, the game server
 * and the master, once their launch options are read.
 */
struct ProgramRun {
  /** @brief The name of the mode, which starts the line of an error:
   * `server stopped: …`. */
  std::string_view label;
  /** @brief The logger the program writes its errors with. */
  std::string_view loggerName;
  /** @brief The file of commands read in the folder the program starts in. */
  std::string_view configurationFile;
  /** @brief The stages of the loop of the program. */
  std::span<const std::string_view> stages;
  /** @brief The lines of `set` the launch options force. */
  std::vector<std::string> forcedLines;
  /** @brief Builds the modules, which tell the StopRequest when they fail. */
  std::function<std::vector<std::unique_ptr<rtype::engine::IModule>>(
      rtype::server::StopRequest&)>
      modules;
  /** @brief Whether it runs until asked to stop, or stops once started. */
  bool waitsForStop{true};
};

/**
 * @brief Tells the operator and the journal that @p program stopped on an
 * error.
 */
void reportFailure(const ProgramRun& program, const std::string& text) {
  const rtype::logging::Logger logger{std::string{program.loggerName}};
  logger.error(program.label, " stopped: ", text);
  std::cerr << program.label << " stopped: " << text << '\n';
}

/**
 * @brief Runs the modules of @p program until Ctrl-C, SIGTERM or a failure
 * asks it to stop, then stops them.
 * @returns the exit code: 0 once the program stopped when asked, 1 after an
 *          error, which is logged and printed. No exception leaves it.
 */
int runUntilStopped(const ProgramRun& program) {
  rtype::server::StopRequest stopRequest;
  const rtype::server::StopSignals stopSignals{stopRequest};
  int exitCode = 0;
  try {
    rtype::engine::ConfigurationReport configurationReport;
    rtype::engine::Application application{
        rtype::engine::LoopStages{program.stages}, program.modules(stopRequest),
        rtype::engine::configurationStep(
            std::filesystem::path{program.configurationFile},
            program.forcedLines, configurationReport)};
    application.start();
    rtype::engine::logConfigurationReport(
        configurationReport, program.configurationFile,
        rtype::logging::Logger{std::string{program.loggerName}});
    const rtype::server::StopReason reason =
        program.waitsForStop ? stopRequest.wait() : rtype::server::StopReason{};
    application.stop();
    if (reason.failed) {
      reportFailure(program, reason.text);
      exitCode = 1;
    }
  } catch (const std::exception& error) {
    reportFailure(program, error.what());
    exitCode = 1;
  }
  stopRequest.announceStopped();
  return exitCode;
}

/**
 * @brief Runs the game server with @p options, or prints its usage.
 * @returns the exit code of the server.
 */
int runServer(std::span<const std::string_view> options) {
  const rtype::server::LaunchOptions launchOptions =
      rtype::server::LaunchOptions::read(options);
  if (launchOptions.helpRequested()) {
    std::cout << rtype::server::LaunchOptions::usage();
    return 0;
  }
  return runUntilStopped(ProgramRun{
      .label = rtype::server::SERVER_MODE_NAME,
      .loggerName = rtype::server::SERVER_LOGGER_NAME,
      .configurationFile = rtype::server::SERVER_CONFIGURATION_FILE_NAME,
      .stages = rtype::server::SERVER_LOOP_STAGES,
      .forcedLines = launchOptions.forcedLines(),
      .modules = [&launchOptions](rtype::server::StopRequest& stopRequest) {
        return rtype::server::serverModules(launchOptions.logOptions(),
                                            launchOptions, stopRequest);
      }});
}

/** @returns How the master runs with @p launchOptions and @p modules. */
ProgramRun masterProgram(
    const rtype::master::MasterLaunchOptions& launchOptions,
    std::function<std::vector<std::unique_ptr<rtype::engine::IModule>>(
        rtype::server::StopRequest&)>
        modules) {
  return ProgramRun{
      .label = rtype::server::MASTER_MODE_NAME,
      .loggerName = rtype::master::MASTER_LOGGER_NAME,
      .configurationFile = rtype::master::MASTER_CONFIGURATION_FILE_NAME,
      .stages = rtype::master::MASTER_LOOP_STAGES,
      .forcedLines = launchOptions.forcedLines(),
      .modules = std::move(modules)};
}

/**
 * @brief Issues a key for the game server @p name with the modules of the
 * master, `http` and `servers` off: the key is printed on the standard output
 * once, never in the journal, and the master stops at once.
 * @returns the exit code: 0 once the key is printed, 1 when none was issued,
 * as when master.cfg turns `server-keys` off.
 */
int issueServerKey(const rtype::master::MasterLaunchOptions& launchOptions,
                   const std::string& name) {
  bool issued = false;
  ProgramRun program = masterProgram(
      launchOptions, [&launchOptions, &name,
                      &issued](rtype::server::StopRequest& stopRequest) {
        return rtype::master::masterModules(
            launchOptions.logOptions(),
            [&stopRequest](const std::string& reason) {
              stopRequest.fail(reason);
            },
            rtype::master::ServerKeyOrder{
                .name = name, .onIssued = [&issued](const std::string& key) {
                  std::cout << key << '\n';
                  issued = true;
                }});
      });
  program.waitsForStop = false;
  std::ranges::copy(rtype::master::keyIssueLines(),
                    std::back_inserter(program.forcedLines));
  const int exitCode = runUntilStopped(program);
  if (exitCode == 0 && !issued) {
    reportFailure(program, std::string{rtype::master::NO_KEY_ISSUED_TEXT});
    return 1;
  }
  return exitCode;
}

/**
 * @brief Runs the master with @p options, issues a key, or prints its usage.
 * @returns the exit code of the master.
 */
int runMaster(std::span<const std::string_view> options) {
  const rtype::master::MasterLaunchOptions launchOptions =
      rtype::master::MasterLaunchOptions::read(options);
  if (launchOptions.helpRequested()) {
    std::cout << rtype::master::MasterLaunchOptions::usage();
    return 0;
  }
  if (const std::optional<std::string>& name = launchOptions.newServerKeyName();
      name.has_value()) {
    return issueServerKey(launchOptions, *name);
  }
  return runUntilStopped(masterProgram(
      launchOptions, [&launchOptions](rtype::server::StopRequest& stopRequest) {
        return rtype::master::masterModules(
            launchOptions.logOptions(),
            [&stopRequest](const std::string& reason) {
              stopRequest.fail(reason);
            },
            std::nullopt);
      }));
}

}  // namespace

int main(int argumentCount, char** arguments) {
  std::string_view programName = rtype::server::PROGRAM_NAME;
  try {
    const std::vector<std::string_view> launchArguments =
        rtype::engine::processArguments(argumentCount, arguments);
    const rtype::server::ServerModeChoice choice =
        rtype::server::chooseServerMode(launchArguments);
    if (choice.mode == rtype::server::ServerMode::Master) {
      programName = rtype::master::MASTER_PROGRAM_NAME;
      return runMaster(choice.options);
    }
    return runServer(choice.options);
  } catch (const std::exception& error) {
    std::cerr << rtype::engine::launchErrorText(programName, error.what());
    return 1;
  }
}
