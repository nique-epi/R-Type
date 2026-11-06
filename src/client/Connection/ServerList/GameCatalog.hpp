#pragma once

#include <cstdint>
#include "GameListSnapshot.hpp"
#include "GameRow.hpp"
#include "IGameCatalog.hpp"
#include "IMasterReads.hpp"
#include "ServerRow.hpp"

namespace rtype::client {

/** @returns Whether a player can join @p game: only a game that waits for its
 * players, since a full one has no seat and one in game takes nobody. */
[[nodiscard]] bool isJoinable(const GameRow& game);

/**
 * @brief The games of the server of the newest ask, read from the master:
 * only the reading of that ask counts, so the games of a server asked before
 * never show under another. It runs on the thread of the frame loop, where
 * update() is called.
 */
class GameCatalog final : public IGameCatalog {
 public:
  /** @param master It must outlive the catalog. */
  explicit GameCatalog(IMasterReads& master);

  void ask(const ServerRow& server) override;
  [[nodiscard]] const GameListSnapshot& snapshot() const override;

  /** @brief Takes the reading of the newest ask once it came. */
  void update();

 private:
  IMasterReads* master_;
  std::uint64_t asked_{};
  std::uint64_t taken_{};
  std::uint8_t maximumPlayers_{};
  GameListSnapshot snapshot_;
};

}  // namespace rtype::client
