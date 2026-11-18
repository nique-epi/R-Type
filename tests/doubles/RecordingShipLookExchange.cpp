#include "RecordingShipLookExchange.hpp"
#include <span>
#include <utility>
#include <vector>
#include "PlayerLooks.hpp"
#include "ShipLook.hpp"

void RecordingShipLookExchange::chooseLook(rtype::identifiers::ShipLook look) {
  chosenLooks_.push_back(look);
}

std::span<const rtype::network::PlayerLook>
RecordingShipLookExchange::playerLooks() const {
  return playerLooks_;
}

void RecordingShipLookExchange::setPlayerLooks(
    std::vector<rtype::network::PlayerLook> looks) {
  playerLooks_ = std::move(looks);
}

const std::vector<rtype::identifiers::ShipLook>&
RecordingShipLookExchange::chosenLooks() const {
  return chosenLooks_;
}
