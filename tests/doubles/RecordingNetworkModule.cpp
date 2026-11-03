#include "RecordingNetworkModule.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include "IDatagramSender.hpp"
#include "IListeningPort.hpp"
#include "IPlayerAdmission.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConstants.hpp"
#include "RecordingDatagramSender.hpp"
#include "RecordingPlayerAdmission.hpp"
#include "TickDatagrams.hpp"

std::string RecordingNetworkModule::name() const {
  return std::string{rtype::server::NETWORK_MODULE_NAME};
}

std::vector<std::string> RecordingNetworkModule::dependencies() const {
  return {};
}

void RecordingNetworkModule::install(rtype::engine::InstallContext& context) {
  context.provide<rtype::server::IDatagramSender>(sender_);
  context.provide<rtype::server::IPlayerAdmission>(admission_);
  context.provide(tickDatagrams_);
  context.provide<rtype::server::IListeningPort>(*this);
}

void RecordingNetworkModule::start() {}

void RecordingNetworkModule::stop() {}

std::uint16_t RecordingNetworkModule::listeningPort() const {
  return rtype::network::DEFAULT_SERVER_PORT;
}

RecordingDatagramSender& RecordingNetworkModule::sender() { return sender_; }

RecordingPlayerAdmission& RecordingNetworkModule::admission() {
  return admission_;
}

rtype::server::TickDatagrams& RecordingNetworkModule::tickDatagrams() {
  return tickDatagrams_;
}
