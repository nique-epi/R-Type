#include "AccountsModule.hpp"
#include <functional>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "AccountRoute.hpp"
#include "AccountsController.hpp"
#include "AccountsModuleConstants.hpp"
#include "AccountsService.hpp"
#include "CommandDefinition.hpp"
#include "CommandInterpreter.hpp"
#include "EngineException.hpp"
#include "InstallContext.hpp"
#include "MasterConstants.hpp"
#include "MasterDatabase.hpp"
#include "MasterHttpServer.hpp"
#include "MasterModuleConstants.hpp"
#include "PasswordKey.hpp"
#include "PasswordKeyText.hpp"
#include "SiteVisitors.hpp"
#include "WireTypes.hpp"

namespace rtype::master {

namespace {

/** @brief Adds every route of ACCOUNT_ROUTES to @p server, answered by
 * @p controller. */
void addRoutes(MasterHttpServer& server, AccountsController& controller) {
  for (const AccountRoute& accountRoute : ACCOUNT_ROUTES) {
    server.add(accountRoute.route,
               std::bind_front(accountRoute.answer, &controller));
  }
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
  AccountsController& controller = controller_.emplace(
      service_.emplace(context.require<MasterDatabase>(), key),
      context.require<SiteVisitors>());
  addRoutes(context.require<MasterHttpServer>(), controller);
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
