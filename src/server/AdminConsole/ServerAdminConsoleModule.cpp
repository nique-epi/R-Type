#include "ServerAdminConsoleModule.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "AdminConsoleParts.hpp"
#include "BanBook.hpp"
#include "BanCommandsParts.hpp"
#include "CommandInterpreter.hpp"
#include "CommandResult.hpp"
#include "CurrentSecond.hpp"
#include "GameRegistry.hpp"
#include "IConsoleInput.hpp"
#include "IConsoleOutput.hpp"
#include "IDatagramSender.hpp"
#include "InstallContext.hpp"
#include "LatestServerMeasures.hpp"
#include "ModuleConstants.hpp"
#include "RejectionLedger.hpp"
#include "ServerConstants.hpp"
#include "TimeConstants.hpp"
#include "TrafficCounters.hpp"

namespace rtype::server {

ServerAdminConsoleModule::ServerAdminConsoleModule(
    std::unique_ptr<IConsoleInput> input,
    std::unique_ptr<IConsoleOutput> output)
    : input_(std::move(input)), output_(std::move(output)) {}

std::string ServerAdminConsoleModule::name() const {
  return std::string{ADMIN_CONSOLE_MODULE_NAME};
}

std::vector<std::string> ServerAdminConsoleModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}, std::string{NETWORK_MODULE_NAME},
          std::string{SUPERVISED_GAMES_MODULE_NAME},
          std::string{BANS_MODULE_NAME}};
}

void ServerAdminConsoleModule::install(engine::InstallContext& context) {
  auto& interpreter = context.require<engine::CommandInterpreter>();
  commands_.emplace(
      AdminConsoleParts{.games = context.require<GameRegistry>(),
                        .sender = context.require<IDatagramSender>(),
                        .traffic = context.require<network::TrafficCounters>(),
                        .rejections = context.require<RejectionLedger>(),
                        .measures = context.require<LatestServerMeasures>(),
                        .interpreter = interpreter,
                        .output = *output_});
  commands_->registerInto(interpreter);
  banCommands_.emplace(BanCommandsParts{.book = context.require<BanBook>(),
                                        .output = *output_,
                                        .now = currentSecond});
  banCommands_->registerInto(interpreter);
  interpreter_ = &interpreter;
  context.addWork(
      SIMULATION_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { runTypedLines(); });
}

void ServerAdminConsoleModule::start() {
  reader_.emplace(*input_, splitter_, lines_);
}

void ServerAdminConsoleModule::stop() { reader_.reset(); }

void ServerAdminConsoleModule::runTypedLines() {
  typed_.clear();
  lines_.drainInto(typed_);
  for (const std::string& line : typed_) {
    const engine::CommandResult result = interpreter_->execute(line);
    if (!result.accepted) {
      output_->write(result.message);
    }
  }
}

}  // namespace rtype::server
