#include "AccountsModule.hpp"
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "AccountsModuleConstants.hpp"
#include "AccountsService.hpp"
#include "CommandDefinition.hpp"
#include "CommandInterpreter.hpp"
#include "ConfigurationRegistry.hpp"
#include "ControllerRoute.hpp"
#include "DeviceLinksService.hpp"
#include "EngineException.hpp"
#include "ISessionAccounts.hpp"
#include "InstallContext.hpp"
#include "MasterConstants.hpp"
#include "MasterDatabase.hpp"
#include "MasterHttpServer.hpp"
#include "MasterModuleConstants.hpp"
#include "PasswordKey.hpp"
#include "PasswordKeyText.hpp"
#include "SettingValue.hpp"
#include "SiteVisitors.hpp"
#include "TextSettingDefinition.hpp"
#include "WireTypes.hpp"

namespace rtype::master {

namespace {

/** @throws engine::CommandRefusedException when @p url starts with neither
 * HTTPS_SCHEME nor HTTP_SCHEME. */
void requireWebAddress(std::string_view url) {
  if (!url.starts_with(HTTPS_SCHEME) && !url.starts_with(HTTP_SCHEME)) {
    throw engine::CommandRefusedException(std::string{SITE_URL_REFUSED_TEXT});
  }
}

/** @returns The `site-url` setting, declared in @p configuration. */
const engine::SettingValue& declareSiteUrl(
    engine::ConfigurationRegistry& configuration) {
  return configuration.declareText(engine::TextSettingDefinition{
      .name = std::string{SITE_URL_SETTING_NAME},
      .defaultValue = std::string{DEFAULT_SITE_URL},
      .help = std::string{SITE_URL_HELP},
      .validate = requireWebAddress});
}

}  // namespace

AccountsModule::AccountsModule(std::string passwordKeyText)
    : passwordKeyText_(std::move(passwordKeyText)),
      logger_(std::string{MASTER_LOGGER_NAME}) {}

std::string AccountsModule::name() const {
  return std::string{ACCOUNTS_MODULE_NAME};
}

std::vector<std::string> AccountsModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}, std::string{STORAGE_MODULE_NAME},
          std::string{HTTP_MODULE_NAME}};
}

void AccountsModule::install(engine::InstallContext& context) {
  const network::PasswordKey key = passwordKeyFromText(passwordKeyText_);
  addAdminCommand(context.require<engine::CommandInterpreter>());
  const engine::SettingValue& siteUrl =
      declareSiteUrl(context.require<engine::ConfigurationRegistry>());
  AccountsService& service =
      service_.emplace(context.require<MasterDatabase>(), key);
  context.provide<ISessionAccounts>(service);
  DeviceLinksService& links =
      links_.emplace(context.require<MasterDatabase>(), service,
                     [&siteUrl] { return siteUrl.text(); });
  auto& server = context.require<MasterHttpServer>();
  const auto& visitors = context.require<SiteVisitors>();
  addRoutes(server, ACCOUNT_ROUTES,
            controller_.emplace(service, links, visitors));
  addRoutes(server, DEVICE_LINK_ROUTES,
            linksController_.emplace(links, service, visitors));
}

void AccountsModule::start() {
  if (!service_.has_value()) {
    return;
  }
  AccountsService& service = *service_;
  for (const std::string& nickname : admins_) {
    makeAdmin(service, nickname);
  }
}

void AccountsModule::stop() {}

void AccountsModule::addAdminCommand(engine::CommandInterpreter& interpreter) {
  interpreter.add(
      engine::CommandDefinition{.name = std::string{ADMIN_COMMAND_NAME},
                                .arguments = std::string{ADMIN_ARGUMENTS},
                                .help = std::string{ADMIN_HELP}},
      [this](std::span<const std::string> words) { nameAdmin(words); });
}

void AccountsModule::nameAdmin(std::span<const std::string> words) {
  if (words.size() != 1 || !network::isValidNickname(words.front())) {
    throw engine::CommandRefusedException(std::string{NOT_ONE_NICKNAME_TEXT});
  }
  admins_.push_back(words.front());
}

void AccountsModule::makeAdmin(AccountsService& service,
                               const std::string& nickname) {
  if (service.makeAdmin(nickname)) {
    logger_.info(nickname, ADMIN_GIVEN_TEXT);
    return;
  }
  logger_.warn(NO_USER_NAMED_TEXT, nickname, ADMIN_LATER_TEXT);
}

}  // namespace rtype::master
