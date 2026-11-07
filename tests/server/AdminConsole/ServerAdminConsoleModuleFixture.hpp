#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Application.hpp"
#include "RecordingConsoleOutput.hpp"
#include "ScriptedConsoleInput.hpp"
#include "SupervisorModulesFixture.hpp"

/** @brief A whole supervisor whose console reads a scripted input and writes
 * to an output the test reads (see SupervisorModulesFixture). */
class ServerAdminConsoleModuleFixture : public SupervisorModulesFixture {
 protected:
  /** @returns The supervisor whose console hands @p chunks, then waits for
   * the stop. Start it once. */
  [[nodiscard]] std::unique_ptr<rtype::engine::Application> supervisorTyping(
      std::vector<std::string> chunks);

  /** @returns The lines the console wrote once it wrote one, running ticks of
   * @p application meanwhile; empty after WAIT_FOR_THREADS. */
  [[nodiscard]] std::vector<std::string> answerOf(
      const rtype::engine::Application& application) const;

  /** @returns Whether the read of the console was woken by the stop. */
  [[nodiscard]] bool consoleWokeOnStop() const;

 private:
  ScriptedConsoleInput* input_{nullptr};
  RecordingConsoleOutput* output_{nullptr};
};
