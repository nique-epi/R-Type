#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"

/** @brief Module double listed after the network module of a server, which
 * keeps the port that module reports while it installs. It opens nothing. */
class PortProbeModule : public rtype::engine::IModule {
 public:
  /** @param dependencies The names of the modules it is listed after. */
  PortProbeModule(std::string name, std::vector<std::string> dependencies);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

  /** @returns The port the IListeningPort reported when install() ran. */
  [[nodiscard]] std::uint16_t portAtInstall() const;

 private:
  std::string name_;
  std::vector<std::string> dependencies_;
  std::uint16_t portAtInstall_{0};
};
