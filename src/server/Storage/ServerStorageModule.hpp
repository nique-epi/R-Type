#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "BanWatch.hpp"
#include "DatabaseLocation.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "ResultsArchive.hpp"

namespace rtype::server {

/**
 * @brief The module that keeps what the games leave: it opens the database of
 * the server when it starts, finishes what waits and closes it when it stops,
 * and provides IResultsArchive. It also reads the bans of that file again
 * every second (see BanWatch), and provides them as LatestBans.
 *
 * install() declares the `database` setting. While it is empty, the file is
 * `r-type_server_<port>.db` in the default folder, the port being the one the
 * IListeningPort of the network module gives; so two servers on one machine
 * never share a file.
 *
 * It depends on the network module, which it reads the port of, so the
 * application refuses a list without it; it is listed before the games module,
 * so it stops after it: the game hands it nothing once it stops, and the game
 * is still there when its thread ends.
 */
class ServerStorageModule final : public engine::IModule {
 public:
  /** @param defaultFolder The folder of the file while the `database` setting
   * is empty: the folder of the executable, for the server. */
  explicit ServerStorageModule(std::filesystem::path defaultFolder);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @brief Declares the `database` setting, requires the IListeningPort, and
   * provides the IResultsArchive and the LatestBans; opens nothing.
   */
  void install(engine::InstallContext& context) override;

  /**
   * @brief Opens the database, starts the storage thread, reads the bans and
   * starts the thread that reads them again.
   * @throws storage::StorageException when the file cannot be opened, which
   * stops the start of the server with an error naming the file; the storage
   * thread is stopped first when only the bans failed.
   */
  void start() override;

  void stop() override;

 private:
  std::filesystem::path defaultFolder_;
  std::unique_ptr<DatabaseLocation> location_;
  logging::Logger logger_;
  ResultsArchive archive_;
  BanWatch banWatch_;
};

}  // namespace rtype::server
