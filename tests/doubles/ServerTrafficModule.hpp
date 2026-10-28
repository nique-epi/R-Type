#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "RecordingServerLink.hpp"
#include "ServerApplication.hpp"

/**
 * @brief Module double that stands for the network module of the server: it
 * is named NETWORK_MODULE_NAME and, during its install(), provides the
 * IServerLink and the datagrams of the tick the test owns. It opens nothing,
 * and the test runs the loop itself.
 */
class ServerTrafficModule : public rtype::engine::IModule {
 public:
  /**
   * @param link Provided as the IServerLink; it must outlive the module.
   * @param received Provided as the datagrams of the tick; it must outlive the
   * module.
   */
  ServerTrafficModule(
      RecordingServerLink& link,
      rtype::server::ServerApplication::ReceivedDatagrams& received);

  /** @returns NETWORK_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  RecordingServerLink* link_;
  rtype::server::ServerApplication::ReceivedDatagrams* received_;
};
