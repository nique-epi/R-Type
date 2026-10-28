#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "LogLaunchOptions.hpp"
#include "Logger.hpp"

namespace rtype::server {

/**
 * @brief The journal of the server, listed first: its start() opens the log
 * file and sets the level the launch options ask for, so every module started
 * after it writes there.
 *
 * Its stop() closes nothing: the Logger keeps its file until the program
 * ends, so the lines written while the other modules stop, and the last line
 * of main(), still reach it.
 */
class LoggingModule : public engine::IModule {
 public:
  explicit LoggingModule(logging::LogLaunchOptions options);

  /** @returns JOURNAL_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns No module: the journal is the first one. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @brief Prepares nothing: the options were read and checked at launch. */
  void install(engine::InstallContext& context) override;
  /**
   * @brief Applies the logging options, then writes that the server starts.
   * @throws LogFileOpenException when the log file cannot be opened.
   */
  void start() override;
  void stop() override;

 private:
  logging::LogLaunchOptions options_;
  logging::Logger logger_;
};

}  // namespace rtype::server
