#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include "IListeningPort.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "ResultsArchive.hpp"
#include "SettingValue.hpp"

namespace rtype::server {

/**
 * @brief The module that keeps what the games leave: it opens the database of
 * the server when it starts, finishes what waits and closes it when it stops,
 * and provides IResultsArchive.
 *
 * install() declares the `database` setting. While it is empty, the file is
 * `r-type_server_<port>.db` in the default folder, the port being the one the
 * IListeningPort of the network module gives; so two servers on one machine
 * never share a file.
 *
 * It depends on the network module, which it reads the port of, so the
 * application refuses a list without it; it is listed before the games module,
 * so it stops after it: no game hands it anything once it stops, and every game
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
   * @brief Declares the `database` setting, requires the IListeningPort and
   * provides the IResultsArchive; opens nothing.
   */
  void install(engine::InstallContext& context) override;

  /**
   * @brief Opens the database and starts the storage thread.
   * @throws storage::StorageException when the file cannot be opened, which
   * stops the start of the server with an error naming the file.
   */
  void start() override;

  void stop() override;

 private:
  [[nodiscard]] std::filesystem::path databaseFile() const;

  std::filesystem::path defaultFolder_;
  const engine::SettingValue* databaseSetting_{nullptr};
  const IListeningPort* listeningPort_{nullptr};
  logging::Logger logger_;
  ResultsArchive archive_;
};

}  // namespace rtype::server
