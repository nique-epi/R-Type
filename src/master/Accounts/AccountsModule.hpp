#pragma once

#include <optional>
#include <span>
#include <string>
#include <vector>
#include "AccountRoute.hpp"
#include "AccountsController.hpp"
#include "AccountsService.hpp"
#include "ApiHandler.hpp"
#include "CommandInterpreter.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "MasterHttpServer.hpp"

namespace rtype::master {

/**
 * @brief The module of the accounts of the players and of their sessions,
 * `POST /api/users`, `GET /api/users/me`, `POST /api/sessions` and
 * `DELETE /api/sessions/me`, kept in the tables `users`, `sessions` and
 * `sign_in_failures`. The routes answer over HTTPS only.
 *
 * It is built with the text of PASSWORD_KEY_VARIABLE, which install() reads:
 * an empty text runs the master without accounts, every route answering 503;
 * a text that is no key stops the master. install() also adds the command
 * `admin <nickname>`, which master.cfg runs before the modules start; start()
 * makes each account it names an admin, once the storage opened the database.
 * The module runs no thread.
 */
class AccountsModule final : public engine::IModule {
 public:
  /** @param passwordKeyText The value of PASSWORD_KEY_VARIABLE, empty when it
   * is unset. */
  explicit AccountsModule(std::string passwordKeyText);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @brief Adds the routes of the accounts and the command `admin`; opens
   * nothing.
   * @throws InvalidPasswordKeyException when the text of the key is not empty
   * and is no key.
   */
  void install(engine::InstallContext& context) override;

  /** @brief Makes an admin of each account an `admin` line named, and writes
   * in the journal what it did of each. */
  void start() override;

  void stop() override;

 private:
  /** @brief Adds every route of ACCOUNT_ROUTES, each answered by the
   * controller, or by AccountsController::closedAnswer() without one. */
  void addRoutes(MasterHttpServer& server);
  [[nodiscard]] network::ApiHandler handlerOf(const AccountRoute& accountRoute);
  void addAdminCommand(engine::CommandInterpreter& interpreter);
  void nameAdmin(std::span<const std::string> words);
  void makeAdmin(AccountsService& service, const std::string& nickname);

  std::string passwordKeyText_;
  std::vector<std::string> admins_;
  std::optional<AccountsService> service_;
  std::optional<AccountsController> controller_;
  logging::Logger logger_;
};

}  // namespace rtype::master
