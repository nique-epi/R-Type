#pragma once

#include <optional>
#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"

/**
 * @brief The module of the greeter plugin, built inside its shared library: it
 * depends on the logger module, requires the ILogJournal that module provides,
 * and writes a line through it when it starts and when it stops.
 */
class GreeterPluginModule : public rtype::engine::IModule {
 public:
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @throws MissingServiceException if the logger module provides no
   * ILogJournal. */
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  std::optional<rtype::logging::Logger> logger_;
};
