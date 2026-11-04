#include "ServerKeysModule.hpp"
#include <hydrogen.h>
#include <chrono>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "InstallContext.hpp"
#include "MasterConstants.hpp"
#include "MasterDatabase.hpp"
#include "MasterModuleConstants.hpp"
#include "ServerKeyOrder.hpp"

namespace rtype::master {

ServerKeysModule::ServerKeysModule(std::optional<ServerKeyOrder> order)
    : order_(std::move(order)), logger_(std::string{MASTER_LOGGER_NAME}) {}

std::string ServerKeysModule::name() const {
  return std::string{SERVER_KEYS_MODULE_NAME};
}

std::vector<std::string> ServerKeysModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}, std::string{STORAGE_MODULE_NAME}};
}

void ServerKeysModule::install(engine::InstallContext& context) {
  keys_.emplace(context.require<MasterDatabase>());
  context.provide(*keys_);
}

void ServerKeysModule::start() {
  static_cast<void>(hydro_init());
  if (order_.has_value() && keys_.has_value()) {
    const std::string key =
        keys_->issue(order_->name, std::chrono::floor<std::chrono::seconds>(
                                       std::chrono::system_clock::now()));
    logger_.info("key issued for the game server ", order_->name);
    order_->onIssued(key);
  }
}

void ServerKeysModule::stop() {}

}  // namespace rtype::master
