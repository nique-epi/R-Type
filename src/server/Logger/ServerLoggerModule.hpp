#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "LogLaunchOptions.hpp"
#include "Logger.hpp"
#include "ProcessJournal.hpp"

namespace rtype::server {

/**
 * @brief The module that opens the journal of the server: it applies the
 * logging options in start(), so every module listed after it logs to the file
 * and to the terminal the options chose, and logs that the server starts and
 * stops.
 *
 * It depends on nothing and is listed first, so it is started first and
 * stopped last: the lines of every other module's start() and stop() are
 * written. It provides the journal of the server as an ILogJournal, so a plugin
 * that names this module in its dependencies logs where the lines of the server
 * go.
 */
class ServerLoggerModule final : public engine::IModule {
 public:
  explicit ServerLoggerModule(logging::LogLaunchOptions options);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @brief Provides the journal of the server as an ILogJournal. It opens
   * nothing: opening the journal keeps a file open, which start() does.
   * @throws DuplicateServiceException if a module installed before already
   * provides an ILogJournal.
   */
  void install(engine::InstallContext& context) override;

  /**
   * @brief Applies the logging options, then logs that the server starts.
   * @throws LogFileOpenException when a journal given with --log-file
   * cannot be opened.
   */
  void start() override;

  /** @brief Logs that the server stopped. */
  void stop() override;

 private:
  logging::LogLaunchOptions options_;
  logging::Logger logger_;
  logging::ProcessJournal journal_;
};

}  // namespace rtype::server
