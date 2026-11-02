#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "ResultsArchive.hpp"

namespace rtype::server {

/**
 * @brief The module that keeps what the games leave: it opens the database of
 * the server when it starts, finishes what waits and closes it when it stops,
 * and provides IResultsArchive.
 *
 * It is listed before the games module, so it stops after it: no game hands it
 * anything once it stops, and every game is still there when its thread ends.
 */
class ServerStorageModule final : public engine::IModule {
 public:
  /** @param databaseFile The file of this server, created at the first start.
   */
  explicit ServerStorageModule(std::filesystem::path databaseFile);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  void install(engine::InstallContext& context) override;

  /**
   * @brief Opens the database and starts the storage thread.
   * @throws storage::StorageException when the file cannot be opened, which
   * stops the start of the server with an error naming the file.
   */
  void start() override;

  void stop() override;

 private:
  std::filesystem::path databaseFile_;
  logging::Logger logger_;
  ResultsArchive archive_;
};

}  // namespace rtype::server
