#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include "GlobalLeaderboard.hpp"
#include "ILeaderboardSource.hpp"
#include "IMasterLeaderboardReads.hpp"
#include "LeaderboardPage.hpp"
#include "LeaderboardPhase.hpp"
#include "LeaderboardRequest.hpp"

namespace rtype::client {

/**
 * @brief The leaderboard of the master, every server together: ask() reads
 * its first MAXIMUM_LEADERBOARD_ROWS rows for the filter, and the row of the
 * player namePlayer() named, through IMasterLeaderboardReads, so the frame
 * never waits on HTTP. Only the reading of the newest ask counts, so the rows
 * of an older filter never show under a newer one. A master that does not
 * answer, refuses or answers what cannot be read gives OutOfReach. It runs on
 * the thread of the frame loop, where update() is called.
 */
class MasterLeaderboard final : public ILeaderboardSource {
 public:
  /** @param master It must outlive the leaderboard. */
  explicit MasterLeaderboard(IMasterLeaderboardReads& master);

  void ask(const network::LeaderboardRequest& request) override;
  void stopAsking() noexcept override;
  [[nodiscard]] const LeaderboardPage& page() const override;

  /** @brief Names the player whose row the next asks add; a nickname the
   * protocol refuses names none. */
  void namePlayer(const std::string& nickname);

  /** @brief Takes the reading of the newest ask once it came. */
  void update();

 private:
  /** @brief Shows the rows of @p leaderboard and the row of its player. */
  void show(const network::GlobalLeaderboard& leaderboard);

  IMasterLeaderboardReads* master_;
  std::optional<std::string> player_;
  /** @brief The number of the ask whose reading the page waits for; nothing
   * once it came, or once the screen stopped asking. */
  std::optional<std::uint64_t> asked_;
  LeaderboardPage page_{.phase = LeaderboardPhase::Asking, .rows = {}};
};

}  // namespace rtype::client
