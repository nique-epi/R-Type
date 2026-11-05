#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "GameListReading.hpp"
#include "GameListing.hpp"
#include "IMasterReads.hpp"
#include "ServerListReading.hpp"
#include "ServerListing.hpp"

/**
 * @brief Master reads double: it writes the asks down and numbers them as
 * MasterLink does, and answers only when the test says so, to the ask the test
 * names, as late as the test wants: an answer to an older ask can come after a
 * newer ask, as it can from MasterLink.
 */
class RecordingMasterReads final : public rtype::client::IMasterReads {
 public:
  std::uint64_t askServers() override;
  std::uint64_t askGames(std::int64_t serverId) override;
  [[nodiscard]] std::optional<rtype::client::ServerListReading> servers()
      const override;
  [[nodiscard]] std::optional<rtype::client::GameListReading> games()
      const override;

  /** @brief Answers the newest ask for servers with @p listing; nothing means
   * the master did not answer. */
  void answerServers(std::optional<rtype::network::ServerListing> listing);
  /** @brief Answers the ask for games numbered @p ask with @p listing. */
  void answerGames(std::uint64_t ask,
                   std::optional<rtype::network::GameListing> listing);

  [[nodiscard]] std::uint64_t serverAskCount() const;
  /** @returns The server of each ask for games, in order. */
  [[nodiscard]] const std::vector<std::int64_t>& gameAsks() const;

 private:
  std::uint64_t serverAsk_{};
  std::vector<std::int64_t> gameAsks_;
  std::optional<rtype::client::ServerListReading> servers_;
  std::optional<rtype::client::GameListReading> games_;
};
