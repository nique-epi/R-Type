#include "ShipLookExchange.hpp"
#include <algorithm>
#include <cstdint>
#include <span>
#include <utility>
#include <vector>
#include "ConnectionConstants.hpp"
#include "ISessionOutbox.hpp"
#include "LobbyState.hpp"
#include "LookItemWon.hpp"
#include "MessageDispatcher.hpp"
#include "MessageOrigin.hpp"
#include "PlayerLooks.hpp"
#include "ShipLook.hpp"
#include "ShipLookChoice.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns Whether @p looks shows @p look on the player numbered @p number. */
bool looksShow(std::span<const network::PlayerLook> looks, std::uint8_t number,
               identifiers::ShipLook look) {
  const identifiers::ShipLook* const own = network::findLook(looks, number);
  return own != nullptr && *own == look;
}

}  // namespace

ShipLookExchange::ShipLookExchange(network::MessageDispatcher& dispatcher,
                                   const IWaitingRoom& room)
    : room_(&room) {
  dispatcher.registerHandler<network::PlayerLooks>(
      [this](const network::MessageOrigin&, const network::PlayerLooks& looks) {
        if (outbox_ != nullptr) {
          looks_ = looks.players;
        }
      });
  dispatcher.registerHandler<network::LookItemWon>(
      [this](const network::MessageOrigin&, const network::LookItemWon& won) {
        wonItems_.push_back(won.item);
      });
}

void ShipLookExchange::chooseLook(identifiers::ShipLook look) {
  if (outbox_ == nullptr || room_->gameStarted() || requestedLook_ == look) {
    return;
  }
  requestedLook_ = look;
  sendChoice(look);
}

std::span<const network::PlayerLook> ShipLookExchange::playerLooks() const {
  return looks_;
}

std::vector<std::uint8_t> ShipLookExchange::takeWonItems() {
  return std::exchange(wonItems_, {});
}

void ShipLookExchange::sessionOpened(const network::LobbyPlayer& ownPlayer,
                                     ISessionOutbox& outbox) {
  outbox_ = &outbox;
  ownPlayerNumber_ = ownPlayer.playerNumber;
}

void ShipLookExchange::sessionClosed() {
  outbox_ = nullptr;
  requestedLook_.reset();
  looks_.clear();
  sinceChoice_ = engine::Duration::zero();
}

void ShipLookExchange::sessionHeld(engine::Duration elapsed) {
  if (!requestedLook_.has_value() || room_->gameStarted() ||
      looksShow(looks_, ownPlayerNumber_, *requestedLook_)) {
    return;
  }
  sinceChoice_ += elapsed;
  if (sinceChoice_ >= SHIP_LOOK_CHOICE_INTERVAL) {
    sendChoice(*requestedLook_);
  }
}

void ShipLookExchange::sendChoice(identifiers::ShipLook look) {
  sendMessage(*outbox_, network::ShipLookChoice{.look = look});
  sinceChoice_ = engine::Duration::zero();
}

}  // namespace rtype::client
