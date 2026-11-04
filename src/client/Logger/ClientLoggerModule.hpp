#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "LogLaunchOptions.hpp"
#include "Logger.hpp"

namespace rtype::client {

/**
 * @brief The module that opens the journal of the client: it applies the
 * logging options in start(), so every module listed after it logs to the file
 * and to the terminal the options chose, then logs that the client starts.
 *
 * It depends on nothing and is listed first, so it is started first and
 * stopped last. Its stop() writes nothing: main() writes the last line once
 * every module stopped, or the error that stopped the client.
 */
class ClientLoggerModule final : public engine::IModule {
 public:
  /** @param options Applied in start(). */
  explicit ClientLoggerModule(logging::LogLaunchOptions options);

  /** @returns LOGGER_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns No module: the journal is the first one. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /** @brief Does nothing: opening the journal keeps a file open, which
   * start() does. */
  void install(engine::InstallContext& context) override;

  /**
   * @brief Applies the logging options, then logs that the client starts.
   * @throws LogFileOpenException when a journal given with --log-file
   * cannot be opened.
   */
  void start() override;

  void stop() override;

 private:
  logging::LogLaunchOptions options_;
  logging::Logger logger_;
};

}  // namespace rtype::client
