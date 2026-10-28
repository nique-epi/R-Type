#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"

/** @brief Module double that tries, during its install(), to bind a UDP socket
 * on a port and releases it at once, and keeps whether the port was free. It
 * opens nothing that outlives install(). */
class PortProbeModule : public rtype::engine::IModule {
 public:
  /** @param dependencies The names of the modules it is listed after. */
  PortProbeModule(std::string name, std::vector<std::string> dependencies,
                  std::uint16_t port);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

  /** @returns Whether the port was free when install() ran. */
  [[nodiscard]] bool portWasFree() const;

 private:
  std::string name_;
  std::vector<std::string> dependencies_;
  std::uint16_t port_;
  bool portWasFree_{false};
};
