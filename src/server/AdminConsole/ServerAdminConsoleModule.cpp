#include "ServerAdminConsoleModule.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "AdminConsoleParts.hpp"
#include "CommandInterpreter.hpp"
#include "CommandResult.hpp"
#include "GameRegistry.hpp"
#include "IConsoleInput.hpp"
#include "IConsoleOutput.hpp"
#include "IDatagramSender.hpp"
#include "ILineCutter.hpp"
#include "ITerminal.hpp"
#include "InstallContext.hpp"
#include "LatestServerMeasures.hpp"
#include "Logger.hpp"
#include "ModuleConstants.hpp"
#include "RejectionLedger.hpp"
#include "ServerConstants.hpp"
#include "TerminalScreen.hpp"
#include "TimeConstants.hpp"
#include "TrafficCounters.hpp"

namespace rtype::server {

ServerAdminConsoleModule::ServerAdminConsoleModule(
    std::unique_ptr<IConsoleInput> input,
    std::unique_ptr<IConsoleOutput> output, std::unique_ptr<ITerminal> terminal)
    : input_(std::move(input)),
      output_(std::move(output)),
      terminal_(std::move(terminal)),
      answers_(*output_) {}

std::string ServerAdminConsoleModule::name() const {
  return std::string{ADMIN_CONSOLE_MODULE_NAME};
}

std::vector<std::string> ServerAdminConsoleModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}, std::string{NETWORK_MODULE_NAME},
          std::string{SUPERVISED_GAMES_MODULE_NAME}};
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
                        .output = answers_});
  commands_->registerInto(interpreter);
  interpreter_ = &interpreter;
  context.addWork(
      SIMULATION_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { runTypedLines(); });
}

void ServerAdminConsoleModule::start() {
  try {
    if (terminal_->enterTypingMode()) {
      showScreen();
    }
    reader_.emplace(*input_, lineCutter(), lines_);
  } catch (...) {
    hideScreen();
    throw;
  }
}

void ServerAdminConsoleModule::stop() {
  reader_.reset();
  hideScreen();
}

void ServerAdminConsoleModule::runTypedLines() {
  typed_.clear();
  lines_.drainInto(typed_);
  for (const std::string& line : typed_) {
    const engine::CommandResult result = interpreter_->execute(line);
    if (!result.accepted) {
      answers_.write(result.message);
    }
  }
}

void ServerAdminConsoleModule::showScreen() {
  TerminalScreen& screen = screen_.emplace(*terminal_);
  screen.open();
  answers_.showOn(&screen);
  logging::setStandardErrorWriter(
      [&screen](std::string_view line) { screen.writeAbove(line); });
}

void ServerAdminConsoleModule::hideScreen() {
  if (screen_.has_value()) {
    logging::setStandardErrorWriter({});
    answers_.showOn(nullptr);
  }
  terminal_->leaveTypingMode();
  if (screen_.has_value()) {
    screen_->close();
    screen_.reset();
  }
}

ILineCutter& ServerAdminConsoleModule::lineCutter() {
  if (screen_.has_value()) {
    return *screen_;
  }
  return splitter_;
}

}  // namespace rtype::server
