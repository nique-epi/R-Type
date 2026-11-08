#include "ShooterViewsFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "Entity.hpp"
#include "EntityTypesTestConstants.hpp"
#include "MissileOwner.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShippedEntitySheets.hpp"
#include "ShooterViews.hpp"
#include "ShooterViewsTestConstants.hpp"
#include "ViewDelay.hpp"

namespace rtype::server::testing {

ShooterViewsFixture::ShooterViewsFixture()
    : shooter_(roster_.add(playerEndpoint(0), std::string{PLAYER_NICKNAME},
                           SHOOTER_SESSION_TOKEN)) {
  static_cast<void>(world_.spawn(game::testing::shippedEntitySheets().named(
                                     game::testing::PLAYER_MISSILE_SHEET),
                                 SOME_MISSILE_PLACE, shooter_->playerNumber));
}

ConnectedPlayer& ShooterViewsFixture::shooter() { return *shooter_; }

void ShooterViewsFixture::giveViewsAt(std::uint32_t tick) {
  giveShooterViews(world_, roster_, tick);
}

std::optional<std::size_t> ShooterViewsFixture::missileDelay() {
  std::optional<std::size_t> delay;
  world_.components().forEach<game::MissileOwner, game::ViewDelay>(
      [&delay](engine::Entity, const game::MissileOwner&,
               const game::ViewDelay& view) { delay = view.ticks; });
  return delay;
}

}  // namespace rtype::server::testing
