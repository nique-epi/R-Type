#include "ClientAccountModule.hpp"
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "ClientConstants.hpp"
#include "IAccountLink.hpp"
#include "IAccountSession.hpp"
#include "IWallet.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ClientAccountModule::ClientAccountModule(std::optional<std::string> launchToken)
    : launchToken_(std::move(launchToken)) {}

std::string ClientAccountModule::name() const {
  return std::string{ACCOUNT_MODULE_NAME};
}

std::vector<std::string> ClientAccountModule::dependencies() const {
  return {std::string{CONNECTION_MODULE_NAME}};
}

void ClientAccountModule::install(engine::InstallContext& context) {
  IAccountLink& link = context.require<IAccountLink>();
  session_.emplace(link);
  wallet_.emplace(*session_, link);
  context.provide<IAccountSession>(*session_);
  context.provide<IWallet>(*wallet_);
  context.addWork(UPDATE_STAGE,
                  [this]([[maybe_unused]] engine::Duration elapsed) {
                    wallet_->followSession();
                  });
}

void ClientAccountModule::start() {
  if (launchToken_.has_value()) {
    session_->signIn(*launchToken_);
  }
}

void ClientAccountModule::stop() {}

}  // namespace rtype::client
