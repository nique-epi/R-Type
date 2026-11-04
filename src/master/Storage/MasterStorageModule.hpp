#pragma once

#include <SQLiteCpp/Database.h>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "SettingValue.hpp"

namespace rtype::master {

/**
 * @brief The module that keeps the database of the master: it opens the file
 * when it starts, applying the MASTER_MIGRATIONS the file does not hold yet,
 * and closes it when it stops.
 *
 * install() declares the `database` setting. While it is empty, the file is
 * DATABASE_FILE_NAME in the default folder. It is listed before the HTTP
 * module, so a file it cannot open stops the master before its port opens.
 */
class MasterStorageModule final : public engine::IModule {
 public:
  /** @param defaultFolder The folder of the file while the `database` setting
   * is empty: the folder of the executable, for the master. */
  explicit MasterStorageModule(std::filesystem::path defaultFolder);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /** @brief Declares the `database` setting; opens nothing. */
  void install(engine::InstallContext& context) override;

  /**
   * @brief Opens the database and applies the migrations it lacks.
   * @throws storage::StorageException when the file cannot be opened or
   * migrated, or holds more migrations than this master knows, which stops
   * the start of the master with an error naming the file.
   */
  void start() override;

  /** @brief Closes the database. */
  void stop() override;

 private:
  [[nodiscard]] std::filesystem::path databaseFile() const;

  std::filesystem::path defaultFolder_;
  const engine::SettingValue* databaseSetting_{nullptr};
  logging::Logger logger_;
  std::optional<SQLite::Database> database_;
};

}  // namespace rtype::master
