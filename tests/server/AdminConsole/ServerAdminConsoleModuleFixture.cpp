#include "ServerAdminConsoleModuleFixture.hpp"
#include <chrono>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>
#include "AdminConsoleTestConstants.hpp"
#include "Application.hpp"
#include "RecordingConsoleOutput.hpp"
#include "RecordingTerminal.hpp"
#include "ScriptedConsoleInput.hpp"
#include "ServerAdminConsoleModule.hpp"
#include "ServerModuleTestConstants.hpp"
#include "TimeConstants.hpp"

std::unique_ptr<rtype::engine::Application>
ServerAdminConsoleModuleFixture::supervisorTyping(
    std::vector<std::string> chunks) {
  return supervisorOn(std::move(chunks),
                      std::make_unique<RecordingTerminal>(false));
}

std::unique_ptr<rtype::engine::Application>
ServerAdminConsoleModuleFixture::supervisorAtATerminal(
    std::vector<std::string> chunks) {
  return supervisorOn(std::move(chunks),
                      std::make_unique<RecordingTerminal>(true));
}

std::unique_ptr<rtype::engine::Application>
ServerAdminConsoleModuleFixture::supervisorOn(
    std::vector<std::string> chunks,
    std::unique_ptr<RecordingTerminal> terminal) {
  auto input = std::make_unique<ScriptedConsoleInput>(std::move(chunks), false);
  auto output = std::make_unique<RecordingConsoleOutput>();
  input_ = input.get();
  output_ = output.get();
  terminal_ = terminal.get();
  return supervisor(
      {}, std::make_unique<rtype::server::ServerAdminConsoleModule>(
              std::move(input), std::move(output), std::move(terminal)));
}

std::vector<std::string> ServerAdminConsoleModuleFixture::answerOf(
    const rtype::engine::Application& application) const {
  const auto deadline = std::chrono::steady_clock::now() + WAIT_FOR_THREADS;
  while (output_->lines().empty() &&
         std::chrono::steady_clock::now() < deadline) {
    application.runStages(rtype::engine::SIMULATION_TICK_DURATION);
    std::this_thread::sleep_for(CONSOLE_LOOK_INTERVAL);
  }
  return output_->lines();
}

std::string ServerAdminConsoleModuleFixture::terminalShowing(
    const rtype::engine::Application& application,
    std::string_view text) const {
  const auto deadline = std::chrono::steady_clock::now() + WAIT_FOR_THREADS;
  while (terminal_->written().find(text) == std::string::npos &&
         std::chrono::steady_clock::now() < deadline) {
    application.runStages(rtype::engine::SIMULATION_TICK_DURATION);
    std::this_thread::sleep_for(CONSOLE_LOOK_INTERVAL);
  }
  return terminal_->written();
}

bool ServerAdminConsoleModuleFixture::terminalInTypingMode() const {
  return terminal_->inTypingMode();
}

bool ServerAdminConsoleModuleFixture::consoleWokeOnStop() const {
  return input_->wokeOnStop();
}
