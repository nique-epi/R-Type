#include "ListeningPortRecordingModule.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include "IListeningPort.hpp"
#include "InstallContext.hpp"
#include "ListeningPortRecordingModuleConstants.hpp"
#include "ModuleConstants.hpp"

ListeningPortRecordingModule::ListeningPortRecordingModule(std::uint16_t& port)
    : port_(&port) {}

std::string ListeningPortRecordingModule::name() const {
  return std::string{LISTENING_PORT_RECORDING_MODULE_NAME};
}

std::vector<std::string> ListeningPortRecordingModule::dependencies() const {
  return {std::string{rtype::server::NETWORK_MODULE_NAME}};
}

void ListeningPortRecordingModule::install(
    rtype::engine::InstallContext& context) {
  listeningPort_ = &context.require<rtype::server::IListeningPort>();
}

void ListeningPortRecordingModule::start() {
  *port_ = listeningPort_->listeningPort();
}

void ListeningPortRecordingModule::stop() {}
