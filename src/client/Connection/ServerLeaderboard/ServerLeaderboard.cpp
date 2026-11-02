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
  if (outbox_ == nullptr) {
    sessionClosed();
    return;
  }
  asked_ = request;
  sinceAsked_ = engine::Duration::zero();
  become(LeaderboardPhase::Asking);
  sendRequest(request);
}

void ServerLeaderboard::stopAsking() { asked_.reset(); }

const LeaderboardPage& ServerLeaderboard::page() const { return page_; }

void ServerLeaderboard::sessionOpened(
    [[maybe_unused]] const network::LobbyPlayer& ownPlayer,
    ISessionOutbox& outbox) {
  outbox_ = &outbox;
}

void ServerLeaderboard::sessionClosed() {
  outbox_ = nullptr;
  asked_.reset();
  become(LeaderboardPhase::NotOnServer);
}

void ServerLeaderboard::sessionHeld(engine::Duration elapsed) {
  if (!asked_.has_value() || page_.phase != LeaderboardPhase::Asking) {
    return;
  }
  sinceAsked_ += elapsed;
  sinceRequest_ += elapsed;
  if (sinceAsked_ >= LEADERBOARD_GIVE_UP_TIME) {
    become(LeaderboardPhase::NoAnswer);
  } else if (sinceRequest_ >= LEADERBOARD_ASK_INTERVAL) {
    sendRequest(*asked_);
  }
}

void ServerLeaderboard::receive(const network::LeaderboardAnswer& answer) {
  if (!asked_.has_value() || page_.phase == LeaderboardPhase::Answered ||
      answer.asked != *asked_) {
    return;
  }
  become(LeaderboardPhase::Answered);
  page_.rows = answer.rows;
  page_.ownRow = answer.ownRow;
}

void ServerLeaderboard::sendRequest(
    const network::LeaderboardRequest& request) {
  sendMessage(*outbox_, request);
  sinceRequest_ = engine::Duration::zero();
}

void ServerLeaderboard::become(LeaderboardPhase phase) {
  page_.rows.clear();
  page_.ownRow.reset();
  page_.phase = phase;
  ++page_.revision;
}

}  // namespace rtype::client
