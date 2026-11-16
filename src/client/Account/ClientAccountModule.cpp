#include "ClientAccountModule.hpp"
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "ClientConstants.hpp"
#include "IAccountLink.hpp"
#include "IAccountSession.hpp"
#include "ICasinoChannel.hpp"
#include "ICasinoLink.hpp"
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
  casino_.emplace(context.require<ICasinoLink>(), *session_, *wallet_);
  context.provide<IAccountSession>(*session_);
  context.provide<IWallet>(*wallet_);
  context.provide<ICasinoChannel>(*casino_);
  context.addWork(UPDATE_STAGE, [this](engine::Duration elapsed) {
    wallet_->followSession();
    casino_->update(elapsed);
  });
}

void ClientAccountModule::start() {
  if (launchToken_.has_value()) {
    session_->signIn(*launchToken_);
  }
}

void ClientAccountModule::stop() {}

}  // namespace rtype::client
