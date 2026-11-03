#include "ListeningPortModule.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include "IListeningPort.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"

ListeningPortModule::ListeningPortModule(std::uint16_t port) : port_(port) {}

std::string ListeningPortModule::name() const {
  return std::string{rtype::server::NETWORK_MODULE_NAME};
}

std::vector<std::string> ListeningPortModule::dependencies() const {
  return {};
}

void ListeningPortModule::install(rtype::engine::InstallContext& context) {
  context.provide<rtype::server::IListeningPort>(*this);
}

void ListeningPortModule::start() {}

void ListeningPortModule::stop() {}

std::uint16_t ListeningPortModule::listeningPort() const { return port_; }
