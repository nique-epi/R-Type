#pragma once

#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "RecordingDatagramSender.hpp"
#include "RecordingPlayerAdmission.hpp"
#include "TickDatagrams.hpp"

/** @brief Module double that stands in for the network module: it provides, as
 * the real one does, a datagram sender and a player admission that record what
 * they receive, and the datagrams of the tick, which the test sets. It opens no
 * socket and runs no thread. */
class RecordingNetworkModule : public rtype::engine::IModule {
 public:
  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(rtype::engine::InstallContext& context) override;
  void start() override;
  void stop() override;

  [[nodiscard]] RecordingDatagramSender& sender();
  [[nodiscard]] RecordingPlayerAdmission& admission();
  [[nodiscard]] rtype::server::TickDatagrams& tickDatagrams();

 private:
  RecordingDatagramSender sender_;
  RecordingPlayerAdmission admission_;
  rtype::server::TickDatagrams tickDatagrams_;
};
