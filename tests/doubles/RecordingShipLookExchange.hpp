#pragma once

#include <cstdint>
#include <span>
#include <vector>
#include "IShipLookExchange.hpp"
#include "PlayerLooks.hpp"
#include "ShipLook.hpp"

/**
 * @brief Look exchange double that writes down every look chosen, in order,
 * and whose looks of the pilots and won items the test sets: a real exchange
 * changes them on its own, at any time between two updates.
 */
class RecordingShipLookExchange : public rtype::client::IShipLookExchange {
 public:
  /** @brief Writes the look down. */
  void chooseLook(rtype::identifiers::ShipLook look) override;
  [[nodiscard]] std::span<const rtype::network::PlayerLook> playerLooks()
      const override;
  /** @brief Hands over the items set with setWonItems(), once. */
  [[nodiscard]] std::vector<std::uint8_t> takeWonItems() override;

  void setPlayerLooks(std::vector<rtype::network::PlayerLook> looks);
  void setWonItems(std::vector<std::uint8_t> items);

  /** @returns The looks chooseLook() was called with, in order. */
  [[nodiscard]] const std::vector<rtype::identifiers::ShipLook>& chosenLooks()
      const;

 private:
  std::vector<rtype::network::PlayerLook> playerLooks_;
  std::vector<std::uint8_t> wonItems_;
  std::vector<rtype::identifiers::ShipLook> chosenLooks_;
};
