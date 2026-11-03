#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "IListeningPort.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"

/** @brief Module double that stands in for the network module as the
 * IListeningPort only: it gives a fixed port and opens nothing. */
class ListeningPortModule : public rtype::engine::IModule,
                            public rtype::server::IListeningPort {
 public:
  explicit ListeningPortModule(std::uint16_t port);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;
  [[nodiscard]] std::uint16_t listeningPort() const override;

 private:
  std::uint16_t port_;
};
