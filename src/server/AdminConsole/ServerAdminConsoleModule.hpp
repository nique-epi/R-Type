#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "AdminConsoleCommands.hpp"
#include "AdminConsoleConstants.hpp"
#include "BoundedQueue.hpp"
#include "CommandInterpreter.hpp"
#include "ConsoleAnswers.hpp"
#include "ConsoleLineSplitter.hpp"
#include "ConsoleReader.hpp"
#include "IConsoleInput.hpp"
#include "IConsoleOutput.hpp"
#include "ILineCutter.hpp"
#include "IModule.hpp"
#include "ITerminal.hpp"
#include "InstallContext.hpp"
#include "TerminalScreen.hpp"

namespace rtype::server {

/**
 * @brief The module admin console of the supervisor, optional: it registers
 * the commands of the console (see AdminConsoleCommands) in the interpreter
 * of the program, then, from start() to stop(), reads its input on the
 * AdminConsole thread (see ConsoleReader). In the simulation stage, each tick,
 * it runs the lines typed since the previous tick, in their order, and writes
 * to its output why a line was refused.
 *
 * When its terminal enters typing mode (the supervisor runs in the foreground
 * of a terminal), it shows a prompt, keeps the line being typed itself, and
 * writes the journal, the lines of the games and the answers above it (see
 * TerminalScreen); otherwise it reads its input line by line, and answers on
 * its output.
 *
 * It is on by default; `module "admin console" off` in server.cfg turns it
 * off, and its commands are then unknown. install() opens nothing.
 */
class ServerAdminConsoleModule final : public engine::IModule {
 public:
  /** @param input What the console reads; the module owns it.
   * @param output Where the commands answer without a terminal; the module
   * owns it.
   * @param terminal The terminal the console types in when it is one (see
   * ITerminal::enterTypingMode()); the module owns it. */
  ServerAdminConsoleModule(std::unique_ptr<IConsoleInput> input,
                           std::unique_ptr<IConsoleOutput> output,
                           std::unique_ptr<ITerminal> terminal);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @brief Registers the commands and adds the work of the tick.
   * @throws MissingServiceException when the network or the supervised games
   * module did not provide what the commands read.
   */
  void install(engine::InstallContext& context) override;

  /** @brief Starts the AdminConsole thread. When the terminal enters typing
   * mode, shows the prompt, and writes the journal and the answers above the
   * typed line until stop(); a start that throws gives the terminal back. */
  void start() override;

  /** @brief Stops the AdminConsole thread, its read in progress woken, then
   * gives the terminal and the journal back. */
  void stop() override;

 private:
  /** @brief Runs the lines typed since the previous tick (SIMULATION_STAGE). */
  void runTypedLines();
  /** @brief Builds the screen, shows it, and routes the journal and the
   * answers to it. */
  void showScreen();
  /** @brief Undoes showScreen(), and gives the terminal its modes back,
   * before it erases the prompt, so a write that fails leaves the terminal
   * as it found it. */
  void hideScreen();
  /** @returns The screen when there is one, the line splitter otherwise. */
  ILineCutter& lineCutter();

  std::unique_ptr<IConsoleInput> input_;
  std::unique_ptr<IConsoleOutput> output_;
  std::unique_ptr<ITerminal> terminal_;
  ConsoleAnswers answers_;
  ConsoleLineSplitter splitter_;
  std::optional<TerminalScreen> screen_;
  concurrency::BoundedQueue<std::string> lines_{CONSOLE_LINE_CAPACITY};
  std::vector<std::string> typed_;
  const engine::CommandInterpreter* interpreter_{nullptr};
  std::optional<AdminConsoleCommands> commands_;
  std::optional<ConsoleReader> reader_;
};

}  // namespace rtype::server
