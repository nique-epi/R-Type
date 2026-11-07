#include "ShooterViews.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "Entity.hpp"
#include "MissileOwner.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "ViewDelay.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

/** @returns How many ticks before @p tick the client of @p player showed the
 * enemies; 0 when it showed none or a tick not before @p tick. */
std::size_t ticksBehind(const ConnectedPlayer* player, std::uint32_t tick) {
  if (player == nullptr || player->appliedShownTick == network::NO_SHOWN_TICK ||
      player->appliedShownTick >= tick) {
    return 0;
  }
  return tick - player->appliedShownTick;
}

/** @brief A missile of a player and the ViewDelay it gets. */
struct MissileView {
  engine::Entity missile;
  game::ViewDelay delay;
};

}  // namespace

void giveShooterViews(game::World& world, PlayerRoster& roster,
                      std::uint32_t tick) {
  engine::ComponentRegistry& components = world.components();
  std::vector<MissileView> views;
  components.forEach<game::MissileOwner, game::MissileOwner>(
      [&roster, &views, tick](engine::Entity missile,
                              const game::MissileOwner& owner,
                              const game::MissileOwner&) {
        views.push_back(MissileView{
            .missile = missile,
            .delay = game::ViewDelay{
                .ticks = ticksBehind(roster.findByNumber(owner.playerNumber),
                                     tick)}});
      });
  for (const MissileView& view : views) {
    components.add(view.missile, view.delay);
  }
}

}  // namespace rtype::server
