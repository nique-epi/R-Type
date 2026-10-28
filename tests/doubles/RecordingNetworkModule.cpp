#include "RecordingNetworkModule.hpp"
#include <string>
#include <vector>
#include "IDatagramSender.hpp"
#include "IPlayerAdmission.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
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
}

void RecordingNetworkModule::start() {}

void RecordingNetworkModule::stop() {}

RecordingDatagramSender& RecordingNetworkModule::sender() { return sender_; }

RecordingPlayerAdmission& RecordingNetworkModule::admission() {
  return admission_;
}

rtype::server::TickDatagrams& RecordingNetworkModule::tickDatagrams() {
  return tickDatagrams_;
}
