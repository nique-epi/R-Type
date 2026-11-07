#include "ServerAdminConsoleModuleFixture.hpp"
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "AdminConsoleTestConstants.hpp"
#include "Application.hpp"
#include "RecordingConsoleOutput.hpp"
#include "ScriptedConsoleInput.hpp"
#include "ServerAdminConsoleModule.hpp"
#include "ServerModuleTestConstants.hpp"
#include "TimeConstants.hpp"

std::unique_ptr<rtype::engine::Application>
ServerAdminConsoleModuleFixture::supervisorTyping(
    std::vector<std::string> chunks) {
  auto input = std::make_unique<ScriptedConsoleInput>(std::move(chunks), false);
  auto output = std::make_unique<RecordingConsoleOutput>();
  input_ = input.get();
  output_ = output.get();
  return supervisor({},
                    std::make_unique<rtype::server::ServerAdminConsoleModule>(
                        std::move(input), std::move(output)));
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

bool ServerAdminConsoleModuleFixture::consoleWokeOnStop() const {
  return input_->wokeOnStop();
}
