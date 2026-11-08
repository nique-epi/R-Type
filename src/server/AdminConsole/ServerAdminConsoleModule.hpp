#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "AdminConsoleCommands.hpp"
#include "AdminConsoleConstants.hpp"
#include "BoundedQueue.hpp"
#include "CommandInterpreter.hpp"
#include "ConsoleReader.hpp"
#include "IConsoleInput.hpp"
#include "IConsoleOutput.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"

namespace rtype::server {

/**
 * @brief The module admin console of the supervisor, optional: it registers
 * the commands of the console (see AdminConsoleCommands) in the interpreter
 * of the program, then, from start() to stop(), reads its input on the
 * AdminConsole thread (see ConsoleReader). In the simulation stage, each tick,
 * it runs the lines typed since the previous tick, in their order, and writes
 * to its output why a line was refused.
 *
 * It is on by default; `module "admin console" off` in server.cfg turns it
 * off, and its commands are then unknown. install() opens nothing.
 */
class ServerAdminConsoleModule final : public engine::IModule {
 public:
  /** @param input What the console reads; the module owns it.
   * @param output Where the commands answer; the module owns it. */
  ServerAdminConsoleModule(std::unique_ptr<IConsoleInput> input,
                           std::unique_ptr<IConsoleOutput> output);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @brief Registers the commands and adds the work of the tick.
   * @throws MissingServiceException when the network or the supervised games
   * module did not provide what the commands read.
   */
  void install(engine::InstallContext& context) override;

  /** @brief Starts the AdminConsole thread. */
  void start() override;

  /** @brief Stops the AdminConsole thread, its read in progress woken. */
  void stop() override;

 private:
  /** @brief Runs the lines typed since the previous tick (SIMULATION_STAGE). */
  void runTypedLines();

  std::unique_ptr<IConsoleInput> input_;
  std::unique_ptr<IConsoleOutput> output_;
  concurrency::BoundedQueue<std::string> lines_{CONSOLE_LINE_CAPACITY};
  std::vector<std::string> typed_;
  const engine::CommandInterpreter* interpreter_{nullptr};
  std::optional<AdminConsoleCommands> commands_;
  std::optional<ConsoleReader> reader_;
};

}  // namespace rtype::server
