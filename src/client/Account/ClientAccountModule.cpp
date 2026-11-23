#include "ClientAccountModule.hpp"
#include <filesystem>
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
#include "SessionTokenFileConstants.hpp"
#include "SettingsFolder.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The file the token is kept in between launches; nothing when
 * `--session` gave the token of this launch, or the system tells no settings
 * folder. */
std::optional<std::filesystem::path> sessionTokenFileLocation(
    const std::optional<std::string>& launchToken) {
  if (launchToken.has_value()) {
    return std::nullopt;
  }
  return settingsFolderFile(SESSION_TOKEN_FILE_NAME);
}

}  // namespace

ClientAccountModule::ClientAccountModule(std::optional<std::string> launchToken)
    : launchToken_(std::move(launchToken)),
      tokenFile_(sessionTokenFileLocation(launchToken_)) {}

std::string ClientAccountModule::name() const {
  return std::string{ACCOUNT_MODULE_NAME};
}

std::vector<std::string> ClientAccountModule::dependencies() const {
  return {std::string{CONNECTION_MODULE_NAME}};
}

void ClientAccountModule::install(engine::InstallContext& context) {
  auto& link = context.require<IAccountLink>();
  session_.emplace(link, tokenFile_);
  wallet_.emplace(*session_, link);
  context.provide<IAccountSession>(*session_);
  context.provide<IWallet>(*wallet_);
  context.addWork(UPDATE_STAGE,
                  [this]([[maybe_unused]] engine::Duration elapsed) {
                    session_->rememberAnswer();
                    wallet_->followSession();
                  });
}

void ClientAccountModule::start() {
  if (!session_.has_value()) {
    return;
  }
  if (launchToken_.has_value()) {
    session_->signIn(*launchToken_);
    return;
  }
  session_->restore();
}

void ClientAccountModule::stop() {}

}  // namespace rtype::client
