#include "ServerTrafficModule.hpp"
#include <string>
#include <vector>
#include "IServerLink.hpp"
#include "InstallContext.hpp"
#include "RecordingServerLink.hpp"
#include "ServerApplication.hpp"
#include "ServerConstants.hpp"

ServerTrafficModule::ServerTrafficModule(
    RecordingServerLink& link,
    rtype::server::ServerApplication::ReceivedDatagrams& received)
    : link_(&link), received_(&received) {}

std::string ServerTrafficModule::name() const {
  return std::string{rtype::server::NETWORK_MODULE_NAME};
}

std::vector<std::string> ServerTrafficModule::dependencies() const {
  return {};
}

void ServerTrafficModule::install(rtype::engine::InstallContext& context) {
  context.provide<rtype::server::IServerLink>(*link_);
  context.provide(*received_);
}

void ServerTrafficModule::start() {}

void ServerTrafficModule::stop() {}
