#pragma once

#include <optional>
#include <span>
#include <string>
#include <vector>
#include "AccountsController.hpp"
#include "AccountsService.hpp"
#include "CommandInterpreter.hpp"
#include "DeviceLinksController.hpp"
#include "DeviceLinksService.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"

namespace rtype::master {

/**
 * @brief The module of the accounts of the players and of their sessions,
 * `POST /api/users`, `GET /api/users/me`, `POST /api/sessions` and
 * `DELETE /api/sessions/me`, kept in the tables `users`, `sessions` and
 * `sign_in_failures`, and of the device links a game signs in through,
 * `/api/device-links`, kept in `device_links`. The routes answer over HTTPS
 * only. install() declares the `site-url` setting, the web site whose page
 * validates a link.
 *
 * It is built with the text of PASSWORD_KEY_VARIABLE, which install() reads:
 * an empty text, or a text that is no key, stops the master before its port
 * opens; a master that must run without accounts turns the module off with
 * `module accounts off`. install() also adds the command
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
   * @brief Declares `site-url`, adds the routes of the accounts and of the
   * device links and the command `admin`; opens nothing.
   * @throws MissingPasswordKeyException when the text of the key is empty.
   * @throws InvalidPasswordKeyException when it is no key.
   */
  void install(engine::InstallContext& context) override;

  /** @brief Makes an admin of each account an `admin` line named, and writes
   * in the journal what it did of each. */
  void start() override;

  void stop() override;

 private:
  void addAdminCommand(engine::CommandInterpreter& interpreter);
  void nameAdmin(std::span<const std::string> words);
  void makeAdmin(AccountsService& service, const std::string& nickname);

  std::string passwordKeyText_;
  std::vector<std::string> admins_;
  std::optional<AccountsService> service_;
  std::optional<AccountsController> controller_;
  std::optional<DeviceLinksService> links_;
  std::optional<DeviceLinksController> linksController_;
  logging::Logger logger_;
};

}  // namespace rtype::master
