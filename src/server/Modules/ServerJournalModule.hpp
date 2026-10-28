#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "LogLaunchOptions.hpp"
#include "Logger.hpp"

namespace rtype::server {

/**
 * @brief The module that opens the journal of the server: it applies the
 * logging options in start(), so every module listed after it logs to the file
 * and to the terminal the options chose, and logs that the server starts and
 * stops.
 *
 * It depends on nothing and is listed first, so it is started first and
 * stopped last: the lines of every other module's start() and stop() are
 * written.
 */
class ServerJournalModule final : public engine::IModule {
 public:
  explicit ServerJournalModule(logging::LogLaunchOptions options);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /** @brief Does nothing: opening the journal keeps a file open, which
   * start() does. */
  void install(engine::InstallContext& context) override;

  /**
   * @brief Applies the logging options, then logs that the server starts.
   * @throws LogFileOpenException when the journal cannot be opened.
   */
  void start() override;

  /** @brief Logs that the server stopped. */
  void stop() override;

 private:
  logging::LogLaunchOptions options_;
  logging::Logger logger_;
};

}  // namespace rtype::server
