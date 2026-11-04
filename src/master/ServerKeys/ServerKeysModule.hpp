#pragma once

#include <optional>
#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "ServerKeyOrder.hpp"
#include "ServerKeys.hpp"

namespace rtype::master {

/**
 * @brief The module of the keys of the game servers: it provides ServerKeys,
 * which the module of the servers reads to tell who registers.
 *
 * With a ServerKeyOrder, start() issues that key and hands it to the order;
 * `--new-server-key` runs the master with this module, the logger and the
 * storage only, so no port opens.
 */
class ServerKeysModule final : public engine::IModule {
 public:
  explicit ServerKeysModule(std::optional<ServerKeyOrder> order);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /** @brief Provides ServerKeys on the MasterDatabase; opens nothing. */
  void install(engine::InstallContext& context) override;

  /**
   * @brief Seeds the random source of libhydrogen on this thread, then issues
   * the key of the order, if any; the journal names the server, never the
   * key.
   * @throws DatabaseClosedException or a SQLite error when the key cannot be
   * saved; nothing is handed to the order then.
   */
  void start() override;

  void stop() override;

 private:
  std::optional<ServerKeyOrder> order_;
  std::optional<ServerKeys> keys_;
  logging::Logger logger_;
};

}  // namespace rtype::master
