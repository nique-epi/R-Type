#pragma once

#include <optional>
#include "ILeaderboardSource.hpp"
#include "ISessionObserver.hpp"
#include "ISessionOutbox.hpp"
#include "LeaderboardAnswer.hpp"
#include "LeaderboardPage.hpp"
#include "LeaderboardPhase.hpp"
#include "LeaderboardRequest.hpp"
#include "LobbyState.hpp"
#include "MessageDispatcher.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The leaderboard of the server of the session: a LeaderboardRequest
 * sent at once, then again every LEADERBOARD_ASK_INTERVAL until a
 * LeaderboardAnswer that lists the same leaderboard arrives, for
 * LEADERBOARD_GIVE_UP_TIME at most. Only that first answer counts, so a late
 * copy or the answer to an older filter changes nothing. The server answers
 * only the players it admitted, so without a session it says NotOnServer and
 * sends nothing.
 */
class ServerLeaderboard final : public ILeaderboardSource,
                                public ISessionObserver {
 public:
  /** @param dispatcher Where the handler of LeaderboardAnswer is registered; it
   * must outlive this object. */
  explicit ServerLeaderboard(network::MessageDispatcher& dispatcher);

  void ask(const network::LeaderboardRequest& request) override;
  void stopAsking() noexcept override;
  [[nodiscard]] const LeaderboardPage& page() const override;

  void sessionOpened(const network::LobbyPlayer& ownPlayer,
                     ISessionOutbox& outbox) override;
  void sessionClosed() override;

  /** @brief Sends the request again, or gives up, as time passes. */
  void sessionHeld(engine::Duration elapsed) override;

 private:
  void receive(const network::LeaderboardAnswer& answer);
  void sendRequest(const network::LeaderboardRequest& request);
  /** @brief Changes the phase, under a new revision, and empties the rows: only
   * an answer, which fills them after this, shows any. */
  void become(LeaderboardPhase phase);

  ISessionOutbox* outbox_{nullptr};
  std::optional<network::LeaderboardRequest> asked_;
  LeaderboardPage page_{.phase = LeaderboardPhase::NotOnServer, .rows = {}};
  engine::Duration sinceAsked_{};
  engine::Duration sinceRequest_{};
};

}  // namespace rtype::client
