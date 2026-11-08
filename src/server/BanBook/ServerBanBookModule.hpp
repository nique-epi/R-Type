#pragma once

#include <string>
#include <vector>
#include "BanBook.hpp"
#include "IModule.hpp"
#include "IResultsFile.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"

namespace rtype::server {

/**
 * @brief The module bans of the supervisor, required: it provides the BanBook
 * the lobby of the supervisor refuses with and the console changes. Its file
 * is the one every game of the supervisor is started with (IResultsFile of the
 * supervised games module), so the games read the bans it writes (see
 * BanWatch). start() reads the bans of that file when it exists (see
 * BanBook::open()); stop() closes it. install() opens nothing.
 */
class ServerBanBookModule final : public engine::IModule {
 public:
  ServerBanBookModule();

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /** @brief Requires the IResultsFile and provides the BanBook.
   * @throws MissingServiceException when no module provided the file. */
  void install(engine::InstallContext& context) override;

  /** @brief Reads the bans in force of the file, when it exists.
   * @throws storage::StorageException when it exists and cannot be opened. */
  void start() override;

  void stop() override;

 private:
  const IResultsFile* file_{nullptr};
  BanBook book_;
  logging::Logger logger_;
};

}  // namespace rtype::server
