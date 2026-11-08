#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "Application.hpp"
#include "RecordingConsoleOutput.hpp"
#include "RecordingTerminal.hpp"
#include "ScriptedConsoleInput.hpp"
#include "SupervisorModulesFixture.hpp"

/** @brief A whole supervisor whose console reads a scripted input, and writes
 * to an output and a terminal the test reads (see SupervisorModulesFixture). */
class ServerAdminConsoleModuleFixture : public SupervisorModulesFixture {
 protected:
  /** @returns The supervisor whose console hands @p chunks, then waits for
   * the stop, on a terminal that never enters typing mode. Start it once. */
  [[nodiscard]] std::unique_ptr<rtype::engine::Application> supervisorTyping(
      std::vector<std::string> chunks);

  /** @returns The same supervisor, on a terminal that enters typing mode. */
  [[nodiscard]] std::unique_ptr<rtype::engine::Application>
  supervisorAtATerminal(std::vector<std::string> chunks);

  /** @returns The lines the console wrote once it wrote one, running ticks of
   * @p application meanwhile; empty after WAIT_FOR_THREADS. */
  [[nodiscard]] std::vector<std::string> answerOf(
      const rtype::engine::Application& application) const;

  /** @returns All the terminal received, once it holds @p text, running ticks
   * of @p application meanwhile; what it holds after WAIT_FOR_THREADS
   * otherwise. */
  [[nodiscard]] std::string terminalShowing(
      const rtype::engine::Application& application,
      std::string_view text) const;

  /** @returns All the terminal received so far, running no tick. */
  [[nodiscard]] std::string terminalWritten() const;

  /** @returns Whether the terminal is in typing mode now. */
  [[nodiscard]] bool terminalInTypingMode() const;

  /** @returns Whether the read of the console was woken by the stop. */
  [[nodiscard]] bool consoleWokeOnStop() const;

 private:
  /** @returns The supervisor whose console hands @p chunks, on @p terminal. */
  [[nodiscard]] std::unique_ptr<rtype::engine::Application> supervisorOn(
      std::vector<std::string> chunks,
      std::unique_ptr<RecordingTerminal> terminal);

  /** @brief Runs ticks of @p application until @p done says so, or
   * WAIT_FOR_THREADS passed. */
  static void runUntil(const rtype::engine::Application& application,
                       const std::function<bool()>& done);

  ScriptedConsoleInput* input_{nullptr};
  RecordingConsoleOutput* output_{nullptr};
  RecordingTerminal* terminal_{nullptr};
};
