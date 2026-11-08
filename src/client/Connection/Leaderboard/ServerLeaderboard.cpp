#include "ServerLeaderboard.hpp"
#include "ConnectionConstants.hpp"
#include "ISessionOutbox.hpp"
#include "LeaderboardAnswer.hpp"
#include "LeaderboardPage.hpp"
#include "LeaderboardPhase.hpp"
#include "LeaderboardRequest.hpp"
#include "LobbyState.hpp"
#include "MessageDispatcher.hpp"
#include "MessageOrigin.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ServerLeaderboard::ServerLeaderboard(network::MessageDispatcher& dispatcher) {
  dispatcher.registerHandler<network::LeaderboardAnswer>(
      [this](const network::MessageOrigin&,
             const network::LeaderboardAnswer& answer) { receive(answer); });
}

void ServerLeaderboard::ask(const network::LeaderboardRequest& request) {
  asked_ = request;
  if (outbox_ == nullptr) {
    enterPhase(page_, LeaderboardPhase::NotOnServer);
    return;
  }
  sinceAsked_ = engine::Duration::zero();
  enterPhase(page_, LeaderboardPhase::Asking);
  sendRequest(request);
}

void ServerLeaderboard::stopAsking() noexcept { asked_.reset(); }

const LeaderboardPage& ServerLeaderboard::page() const { return page_; }

void ServerLeaderboard::sessionOpened(
    [[maybe_unused]] const network::LobbyPlayer& ownPlayer,
    ISessionOutbox& outbox) {
  outbox_ = &outbox;
  if (asked_.has_value()) {
    sinceAsked_ = engine::Duration::zero();
    sinceRequest_ = LEADERBOARD_ASK_INTERVAL;
    enterPhase(page_, LeaderboardPhase::Asking);
  }
}

void ServerLeaderboard::sessionClosed() {
  outbox_ = nullptr;
  enterPhase(page_, LeaderboardPhase::NotOnServer);
}

void ServerLeaderboard::sessionHeld(engine::Duration elapsed) {
  if (!asked_.has_value() || page_.phase != LeaderboardPhase::Asking) {
    return;
  }
  sinceAsked_ += elapsed;
  sinceRequest_ += elapsed;
  if (sinceAsked_ >= LEADERBOARD_GIVE_UP_TIME) {
    enterPhase(page_, LeaderboardPhase::NoAnswer);
  } else if (sinceRequest_ >= LEADERBOARD_ASK_INTERVAL) {
    sendRequest(*asked_);
  }
}

void ServerLeaderboard::receive(const network::LeaderboardAnswer& answer) {
  if (!asked_.has_value() || page_.phase == LeaderboardPhase::Answered ||
      answer.asked != *asked_) {
    return;
  }
  enterPhase(page_, LeaderboardPhase::Answered);
  page_.rows = answer.rows;
  page_.ownRow = answer.ownRow;
}

void ServerLeaderboard::sendRequest(
    const network::LeaderboardRequest& request) {
  sendMessage(*outbox_, request);
  sinceRequest_ = engine::Duration::zero();
}

}  // namespace rtype::client
