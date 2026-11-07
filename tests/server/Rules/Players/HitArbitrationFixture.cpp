#include "HitArbitrationFixture.hpp"
#include <cstddef>
#include <optional>
#include <string>
#include "AppliedInput.hpp"
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "Entity.hpp"
#include "EntitySheets.hpp"
#include "EntityType.hpp"
#include "EntityTypesTestConstants.hpp"
#include "IHitArbitration.hpp"
#include "MissileOwner.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShippedEntitySheets.hpp"
#include "ShooterViewsTestConstants.hpp"
#include "ViewDelay.hpp"

namespace rtype::server::testing {

HitArbitrationFixture::HitArbitrationFixture()
    : player_(roster_.add(playerEndpoint(0), std::string{PLAYER_NICKNAME},
                          SHOOTER_SESSION_TOKEN)) {
  const game::EntitySheets& sheets = game::testing::shippedEntitySheets();
  player_->shipEntityId =
      world_.spawn(sheets.named(game::testing::SHIP_SHEET), SOME_SHIP_PLACE);
  static_cast<void>(
      world_.spawn(sheets.named(game::testing::PLAYER_MISSILE_SHEET),
                   SOME_MISSILE_PLACE, player_->playerNumber));
  player_->appliedInput = AppliedInput{.inputNumber = SHOOTER_INPUT_NUMBER,
                                       .shownTick = TICK_FIVE_BEFORE};
}

void HitArbitrationFixture::giveViewsOf(const IHitArbitration& arbitration) {
  arbitration.giveViews(world_, roster_, PRESENT_TICK);
}

std::optional<std::size_t> HitArbitrationFixture::shipDelay() {
  std::optional<std::size_t> delay;
  world_.components().forEach<game::EntityType, game::ViewDelay>(
      [&delay](engine::Entity, const game::EntityType& side,
               const game::ViewDelay& view) {
        if (side == game::EntityType::Player) {
          delay = view.ticks;
        }
      });
  return delay;
}

std::optional<std::size_t> HitArbitrationFixture::missileDelay() {
  std::optional<std::size_t> delay;
  world_.components().forEach<game::MissileOwner, game::ViewDelay>(
      [&delay](engine::Entity, const game::MissileOwner&,
               const game::ViewDelay& view) { delay = view.ticks; });
  return delay;
}

}  // namespace rtype::server::testing
