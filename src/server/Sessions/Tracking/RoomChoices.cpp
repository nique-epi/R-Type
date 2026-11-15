#include "RoomChoices.hpp"
#include <utility>
#include "CasinoPlace.hpp"
#include "ConnectedPlayer.hpp"
#include "MessageDispatcher.hpp"
#include "MessageOrigin.hpp"
#include "PlayerRoster.hpp"
#include "ReadyKeeper.hpp"
#include "RoomChoiceHandlers.hpp"
#include "RouletteBets.hpp"
#include "ShipLookChoice.hpp"

namespace rtype::server {

RoomChoices::RoomChoices(PlayerRoster& roster, const ReadyKeeper& ready,
                         RoomChoiceHandlers handlers)
    : roster_(roster), ready_(ready), handlers_(std::move(handlers)) {}

void RoomChoices::registerWith(network::MessageDispatcher& dispatcher) {
  dispatcher.registerHandler<network::ShipLookChoice>(
      [this](const network::MessageOrigin& origin,
             const network::ShipLookChoice& choice) {
        if (ConnectedPlayer* const player = chooserOf(origin);
            player != nullptr) {
          handlers_.onShipLookChoice(*player, choice);
        }
      });
  dispatcher.registerHandler<network::CasinoPlace>(
      [this](const network::MessageOrigin& origin,
             const network::CasinoPlace& place) {
        if (ConnectedPlayer* const player = chooserOf(origin);
            player != nullptr) {
          handlers_.onCasinoPlace(*player, place);
        }
      });
  dispatcher.registerHandler<network::RouletteBets>(
      [this](const network::MessageOrigin& origin,
             const network::RouletteBets& bets) {
        if (const ConnectedPlayer* const player = chooserOf(origin);
            player != nullptr) {
          handlers_.onRouletteBets(*player, bets);
        }
      });
}

ConnectedPlayer* RoomChoices::chooserOf(const network::MessageOrigin& origin) {
  if (!ready_.acceptsRoomChoices()) {
    return nullptr;
  }
  return roster_.find(origin.sender);
}

}  // namespace rtype::server
