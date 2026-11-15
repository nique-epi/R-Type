#pragma once

#include <functional>
#include "CasinoPlace.hpp"
#include "ConnectedPlayer.hpp"
#include "RouletteBets.hpp"
#include "ShipLookChoice.hpp"

namespace rtype::server {

/**
 * @brief What is called with the choices a player makes in the waiting room,
 * copies included, while the game takes them. None may be empty.
 */
struct RoomChoiceHandlers {
  /** @brief Called with the player and each ShipLookChoice it sends, for
   * whoever shares the looks. */
  std::function<void(ConnectedPlayer&, const network::ShipLookChoice&)>
      onShipLookChoice;
  /** @brief Called with the player and each CasinoPlace it sends, for whoever
   * keeps the roulette table. */
  std::function<void(ConnectedPlayer&, const network::CasinoPlace&)>
      onCasinoPlace;
  /** @brief Called with the player and each RouletteBets it sends, for whoever
   * keeps the roulette table. */
  std::function<void(const ConnectedPlayer&, const network::RouletteBets&)>
      onRouletteBets;
};

}  // namespace rtype::server
