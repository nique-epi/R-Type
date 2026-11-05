#include "ClientConnectionModule.hpp"
#include <exception>
#include <string>
#include <string_view>
#include <vector>
#include "ClientConstants.hpp"
#include "ConnectionConstants.hpp"
#include "EntityAppearances.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConditions.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ClientConnectionModule::ClientConnectionModule(
    const network::NetworkConditions& conditions)
    : connection_(conditions,
                  [this](std::string_view name) {
                    return appearances_ != nullptr &&
                           appearances_->named(name) != nullptr;
                  }),
      logger_(std::string{CONNECTION_LOGGER_NAME}) {}

std::string ClientConnectionModule::name() const {
  return std::string{CONNECTION_MODULE_NAME};
}

std::vector<std::string> ClientConnectionModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}, std::string{ASSETS_MODULE_NAME}};
}

void ClientConnectionModule::install(engine::InstallContext& context) {
  appearances_ = &context.require<EntityAppearances>();
  connection_.provideParts(context);
  context.addWork(NETWORK_STAGE, [this](engine::Duration elapsed) {
    connection_.update(elapsed);
  });
}

void ClientConnectionModule::start() { connection_.network().open(); }

void ClientConnectionModule::stop() {
  try {
    connection_.leave();
  } catch (const std::exception& error) {
    logger_.warn("the server was not told the client leaves: ", error.what());
  }
  connection_.network().close();
}

}  // namespace rtype::client
