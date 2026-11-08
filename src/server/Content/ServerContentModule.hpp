#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include "ContentPackDesk.hpp"
#include "HttpServer.hpp"
#include "IListeningPort.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "SystemClock.hpp"

namespace rtype::server {

/**
 * @brief The module content of the supervisor, required: it serves the
 * content pack of this server, the sheets of its assets folder and the sprite
 * sheets they name, at network::CONTENT_PACK_ROUTE, so a client that lacks
 * them downloads them before it plays.
 *
 * install() reads the content folders and encodes the pack once. start()
 * listens for HTTP on every IPv4 interface and on the TCP port of the number
 * the players reach the server by in UDP. When that port is taken, or the
 * system chooses the UDP port, the server runs without serving the pack and
 * the journal says so: a player who has the content of the server still
 * plays, the others are told the download failed.
 */
class ServerContentModule final : public engine::IModule {
 public:
  /** @param contentFolders The folders the pack is made of, the assets first
   * then each resource the games start (see engine::contentFoldersOf()). */
  explicit ServerContentModule(
      std::vector<std::filesystem::path> contentFolders);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @brief Reads the content of the content folders, encodes the pack and adds
   * its route; opens nothing.
   * @throws What game::contentFilesIn() and network::encodeContentPack()
   * throw, when the content cannot be read or would make no valid pack.
   */
  void install(engine::InstallContext& context) override;

  /** @brief Listens for HTTP, or tells the journal why it does not. */
  void start() override;

  /** @brief Stops listening, once the downloads in progress are answered. */
  void stop() override;

 private:
  std::vector<std::filesystem::path> contentFolders_;
  engine::SystemClock clock_;
  logging::Logger logger_;
  network::HttpServer server_;
  std::optional<ContentPackDesk> desk_;
  const IListeningPort* port_{nullptr};
};

}  // namespace rtype::server
