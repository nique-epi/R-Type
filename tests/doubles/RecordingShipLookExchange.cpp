#include "RecordingShipLookExchange.hpp"
#include <cstdint>
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

std::vector<std::uint8_t> RecordingShipLookExchange::takeWonItems() {
  return std::exchange(wonItems_, {});
}

void RecordingShipLookExchange::setPlayerLooks(
    std::vector<rtype::network::PlayerLook> looks) {
  playerLooks_ = std::move(looks);
}

void RecordingShipLookExchange::setWonItems(std::vector<std::uint8_t> items) {
  wonItems_ = std::move(items);
}

const std::vector<rtype::identifiers::ShipLook>&
RecordingShipLookExchange::chosenLooks() const {
  return chosenLooks_;
}
