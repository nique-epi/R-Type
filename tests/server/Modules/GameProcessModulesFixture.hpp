#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "Application.hpp"
#include "ConfigurationReport.hpp"
#include "CurrentDirectoryChange.hpp"
#include "GameLaunchOptions.hpp"
#include "MessageType.hpp"
#include "ServerModuleFixture.hpp"
#include "StopRequest.hpp"
#include "TemporaryFolder.hpp"
#include "UdpTestClient.hpp"

/** @brief The base of the tests of a whole game process: its launch options on
 * a free port, linked to a supervisor that only receives, an empty folder as
 * the working folder (the game finds its assets from its executable, next to
 * which the build copies the waves file), and a client that can join. */
class GameProcessModulesFixture : public ServerModuleFixture {
 public:
  GameProcessModulesFixture();

 protected:
  /** @brief Has the client join the server through the whole handshake.
   * @returns The session token the server gave it. */
  [[nodiscard]] std::uint64_t joinWithTheClient();

  /** @returns The datagrams of @p received that hold a message of @p type,
   * oldest first. */
  [[nodiscard]] static std::vector<ReceivedAt> holding(
      const std::vector<ReceivedAt>& received,
      rtype::network::MessageType type);

  [[nodiscard]] std::uint16_t port() const;
  [[nodiscard]] rtype::server::StopRequest& stopRequest();
  [[nodiscard]] UdpTestClient& client();
  [[nodiscard]] const rtype::server::GameLaunchOptions& launchOptions() const;

  /** @returns The arguments of a game process on port() linked to the
   * supervisor of the fixture, with its database in the working folder and its
   * journal in the journal of the test only, followed by @p more. */
  [[nodiscard]] std::vector<std::string_view> gameArguments(
      const std::vector<std::string_view>& more) const;

  /** @returns The modules of a whole game process read from @p options, with
   * the configuration step main() gives them: it reads the file of
   * `--configuration`, none without it, and the launch options set the port
   * and the database through its forced lines. Start it once. */
  [[nodiscard]] std::unique_ptr<rtype::engine::Application> gameApplication(
      const rtype::server::GameLaunchOptions& options);

  /** @brief Writes @p lines, one per line, to @p file of the folder the game
   * starts in. */
  void writeConfiguration(std::string_view file,
                          const std::vector<std::string>& lines) const;

 private:
  std::uint16_t port_;
  rtype::server::StopRequest stopRequest_;
  UdpTestClient client_;
  UdpTestClient supervisor_;
  TemporaryFolder workingFolder_;
  CurrentDirectoryChange workingFolderChange_;
  std::string portText_;
  std::string supervisorPortText_;
  std::string databaseText_;
  std::string journalText_;
  rtype::server::GameLaunchOptions launchOptions_;
  rtype::engine::ConfigurationReport configurationReport_;
};
