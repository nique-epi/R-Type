#include <exception>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <span>
#include <string>
#include <string_view>
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
#include "ServerMode.hpp"
#include "ServerModeChoice.hpp"
#include "ServerModeOption.hpp"
#include "ServerModules.hpp"
#include "StopReason.hpp"
#include "StopRequest.hpp"
#include "StopSignals.hpp"
#include "SupervisorModules.hpp"

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
    const rtype::server::StopReason reason = stopRequest.wait();
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

/** @brief Builds the modules of a program of the server binary from its launch
 * options. */
using ServerModulesBuilder =
    std::function<std::vector<std::unique_ptr<rtype::engine::IModule>>(
        const rtype::server::LaunchOptions&, rtype::server::StopRequest&)>;

/**
 * @brief Runs a program that takes the options of the game server, named
 * @p label, with @p options, or prints its usage.
 * @returns the exit code of the program.
 */
int runServerProgram(std::string_view label,
                     std::span<const std::string_view> options,
                     const ServerModulesBuilder& build) {
  const rtype::server::LaunchOptions launchOptions =
      rtype::server::LaunchOptions::read(options);
  if (launchOptions.helpRequested()) {
    std::cout << rtype::server::LaunchOptions::usage();
    return 0;
  }
  return runUntilStopped(ProgramRun{
      .label = label,
      .loggerName = rtype::server::SERVER_LOGGER_NAME,
      .configurationFile = rtype::server::SERVER_CONFIGURATION_FILE_NAME,
      .stages = rtype::server::SERVER_LOOP_STAGES,
      .forcedLines = launchOptions.forcedLines(),
      .modules = [&launchOptions,
                  &build](rtype::server::StopRequest& stopRequest) {
        return build(launchOptions, stopRequest);
      }});
}

/**
 * @brief Runs the game server with @p options, or prints its usage.
 * @returns the exit code of the server.
 */
int runServer(std::span<const std::string_view> options) {
  return runServerProgram(rtype::server::SERVER_MODE_NAME, options,
                          [](const rtype::server::LaunchOptions& launchOptions,
                             rtype::server::StopRequest& stopRequest) {
                            return rtype::server::serverModules(
                                launchOptions.logOptions(), launchOptions,
                                stopRequest);
                          });
}

/**
 * @brief Runs the supervisor with @p options, or prints its usage.
 * @returns the exit code of the supervisor.
 */
int runSupervisor(std::span<const std::string_view> options) {
  return runServerProgram(rtype::server::SUPERVISOR_MODE_NAME, options,
                          rtype::server::supervisorModules);
}

/**
 * @brief Runs the master with @p options, or prints its usage.
 * @returns the exit code of the master.
 */
int runMaster(std::span<const std::string_view> options) {
  const rtype::master::MasterLaunchOptions launchOptions =
      rtype::master::MasterLaunchOptions::read(options);
  if (launchOptions.helpRequested()) {
    std::cout << rtype::master::MasterLaunchOptions::usage();
    return 0;
  }
  return runUntilStopped(ProgramRun{
      .label = rtype::server::MASTER_MODE_NAME,
      .loggerName = rtype::master::MASTER_LOGGER_NAME,
      .configurationFile = rtype::master::MASTER_CONFIGURATION_FILE_NAME,
      .stages = rtype::master::MASTER_LOOP_STAGES,
      .forcedLines = launchOptions.forcedLines(),
      .modules = [&launchOptions](rtype::server::StopRequest& stopRequest) {
        return rtype::master::masterModules(
            launchOptions.logOptions(),
            [&stopRequest](const std::string& reason) {
              stopRequest.fail(reason);
            });
      }});
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
    if (choice.mode == rtype::server::ServerMode::Supervisor) {
      return runSupervisor(choice.options);
    }
    return runServer(choice.options);
  } catch (const std::exception& error) {
    std::cerr << rtype::engine::launchErrorText(programName, error.what());
    return 1;
  }
}
