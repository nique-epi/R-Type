#include <exception>
#include <filesystem>
#include <functional>
#include <iostream>
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
#include "ConfigurationSource.hpp"
#include "ConfigurationStep.hpp"
#include "GameLaunchOptions.hpp"
#include "GameLineRule.hpp"
#include "GameProcessModules.hpp"
#include "HandOverRule.hpp"
#include "IModule.hpp"
#include "LaunchArguments.hpp"
#include "LaunchOptions.hpp"
#include "Logger.hpp"
#include "LoopStages.hpp"
#include "MasterConstants.hpp"
#include "MasterLaunchOptions.hpp"
#include "MasterModules.hpp"
#include "PathText.hpp"
#include "ServerConstants.hpp"
#include "ServerMode.hpp"
#include "ServerModeChoice.hpp"
#include "ServerModeOption.hpp"
#include "StopReason.hpp"
#include "StopRequest.hpp"
#include "StopSignals.hpp"
#include "SupervisorModules.hpp"

namespace {

/** @brief The modules of a program, in the order they install and start. */
using ProgramModules = std::vector<std::unique_ptr<rtype::engine::IModule>>;

/**
 * @brief What tells apart the programs the binary runs, the supervisor, a game
 * process and the master, once their launch options are read.
 */
struct ProgramRun {
  /** @brief The name of the mode, which starts the line of an error:
   * `game stopped: …`. */
  std::string_view label;
  /** @brief The logger the program writes its errors with. */
  std::string_view loggerName;
  /** @brief The file of commands the program reads; empty to read none. */
  std::filesystem::path configurationFile;
  /** @brief The stages of the loop of the program. */
  std::span<const std::string_view> stages;
  /** @brief The lines of `set` the launch options force. */
  std::vector<std::string> forcedLines;
  /** @brief Builds the modules, which tell the StopRequest when they fail; the
   * report is filled by the configuration step, before the modules start. */
  std::function<ProgramModules(rtype::server::StopRequest&,
                               const rtype::engine::ConfigurationReport&)>
      modules;
  /** @brief Gives, from the modules built, the rule of the lines that belong
   * to another program; empty for a program that hands no line over. */
  std::function<rtype::engine::HandOverRule(
      std::span<const std::unique_ptr<rtype::engine::IModule>>)>
      handOver;
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
 * @returns The configuration step of @p program, which reads its file and its
 * forced lines into @p report and hands over the lines its rule gives to
 * another program, @p modules being the modules it was built with.
 */
rtype::engine::ConfigurationStep configurationStepOf(
    const ProgramRun& program,
    std::span<const std::unique_ptr<rtype::engine::IModule>> modules,
    rtype::engine::ConfigurationReport& report) {
  return rtype::engine::configurationStep(
      rtype::engine::ConfigurationSource{
          .file = program.configurationFile,
          .forcedLines = program.forcedLines,
          .handOver = program.handOver ? program.handOver(modules)
                                       : rtype::engine::HandOverRule{}},
      report);
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
    ProgramModules modules = program.modules(stopRequest, configurationReport);
    rtype::engine::ConfigurationStep step =
        configurationStepOf(program, modules, configurationReport);
    rtype::engine::Application application{
        rtype::engine::LoopStages{program.stages}, std::move(modules),
        std::move(step)};
    application.start();
    rtype::engine::logConfigurationReport(
        configurationReport,
        rtype::logging::genericText(program.configurationFile),
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

/**
 * @returns The launch options @p options give, read with Options; nothing
 * when they ask for the usage, which is then printed.
 */
template <typename Options>
std::optional<Options> launchOptionsOrUsage(
    std::span<const std::string_view> options) {
  Options launchOptions = Options::read(options);
  if (launchOptions.helpRequested()) {
    std::cout << Options::usage();
    return std::nullopt;
  }
  return launchOptions;
}

/**
 * @brief Runs a game process with @p options, or prints its usage. The game
 * reads the file its supervisor gave after `--configuration`, and no file
 * without it.
 * @returns the exit code of the game.
 */
int runGame(std::span<const std::string_view> options) {
  const std::optional<rtype::server::GameLaunchOptions> launchOptions =
      launchOptionsOrUsage<rtype::server::GameLaunchOptions>(options);
  if (!launchOptions.has_value()) {
    return 0;
  }
  return runUntilStopped(ProgramRun{
      .label = rtype::server::GAME_MODE_NAME,
      .loggerName = rtype::server::SERVER_LOGGER_NAME,
      .configurationFile =
          launchOptions->configurationFile().value_or(std::filesystem::path{}),
      .stages = rtype::server::SERVER_LOOP_STAGES,
      .forcedLines = launchOptions->forcedLines(),
      .modules =
          [&launchOptions](rtype::server::StopRequest& stopRequest,
                           const rtype::engine::ConfigurationReport&) {
            return rtype::server::gameProcessModules(*launchOptions,
                                                     stopRequest);
          },
      .handOver = {}});
}

/**
 * @brief Runs the supervisor with @p options, or prints its usage. It reads
 * server.cfg, runs its own lines and hands the lines of the games to its
 * supervised games module, which writes them for every game.
 * @returns the exit code of the supervisor.
 */
int runSupervisor(std::span<const std::string_view> options) {
  const std::optional<rtype::server::LaunchOptions> launchOptions =
      launchOptionsOrUsage<rtype::server::LaunchOptions>(options);
  if (!launchOptions.has_value()) {
    return 0;
  }
  return runUntilStopped(ProgramRun{
      .label = rtype::server::SUPERVISOR_MODE_NAME,
      .loggerName = rtype::server::SERVER_LOGGER_NAME,
      .configurationFile =
          std::filesystem::path{rtype::server::SERVER_CONFIGURATION_FILE_NAME},
      .stages = rtype::server::SERVER_LOOP_STAGES,
      .forcedLines = launchOptions->forcedLines(),
      .modules =
          [&launchOptions](rtype::server::StopRequest& stopRequest,
                           const rtype::engine::ConfigurationReport& report) {
            return rtype::server::supervisorModules(*launchOptions, stopRequest,
                                                    report.handedOverLines);
          },
      .handOver = rtype::server::gameLineRule});
}

/**
 * @brief Runs the master with @p options, or prints its usage.
 * @returns the exit code of the master.
 */
int runMaster(std::span<const std::string_view> options) {
  const std::optional<rtype::master::MasterLaunchOptions> launchOptions =
      launchOptionsOrUsage<rtype::master::MasterLaunchOptions>(options);
  if (!launchOptions.has_value()) {
    return 0;
  }
  return runUntilStopped(ProgramRun{
      .label = rtype::server::MASTER_MODE_NAME,
      .loggerName = rtype::master::MASTER_LOGGER_NAME,
      .configurationFile =
          std::filesystem::path{rtype::master::MASTER_CONFIGURATION_FILE_NAME},
      .stages = rtype::master::MASTER_LOOP_STAGES,
      .forcedLines = launchOptions->forcedLines(),
      .modules =
          [&launchOptions](rtype::server::StopRequest& stopRequest,
                           const rtype::engine::ConfigurationReport&) {
            return rtype::master::masterModules(
                launchOptions->logOptions(),
                [&stopRequest](const std::string& reason) {
                  stopRequest.fail(reason);
                });
          },
      .handOver = {}});
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
    if (choice.mode == rtype::server::ServerMode::Game) {
      return runGame(choice.options);
    }
    return runSupervisor(choice.options);
  } catch (const std::exception& error) {
    std::cerr << rtype::engine::launchErrorText(programName, error.what());
    return 1;
  }
}
