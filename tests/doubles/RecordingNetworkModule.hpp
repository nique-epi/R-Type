#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "IListeningPort.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "RecordingDatagramSender.hpp"
#include "RecordingPlayerAdmission.hpp"
#include "TickDatagrams.hpp"

/** @brief Module double that stands in for the network module: it provides, as
 * the real one does, a datagram sender and a player admission that record what
 * they receive, the datagrams of the tick, which the test sets, and the default
 * port as the port it listens on. It opens no socket and runs no thread. */
class RecordingNetworkModule : public rtype::engine::IModule,
                               public rtype::server::IListeningPort {
 public:
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;
  [[nodiscard]] std::uint16_t listeningPort() const override;

  [[nodiscard]] RecordingDatagramSender& sender();
  [[nodiscard]] RecordingPlayerAdmission& admission();
  [[nodiscard]] rtype::server::TickDatagrams& tickDatagrams();

 private:
  RecordingDatagramSender sender_;
  RecordingPlayerAdmission admission_;
  rtype::server::TickDatagrams tickDatagrams_;
};
