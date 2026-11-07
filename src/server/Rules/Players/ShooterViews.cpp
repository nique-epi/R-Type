#include "ShooterViews.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "AppliedInput.hpp"
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "MissileOwner.hpp"
#include "NetworkIdentity.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "ViewDelay.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

/** @returns How many ticks before @p tick the client of @p player showed the
 * enemies; 0 when it showed none or a tick not before @p tick. */
std::size_t ticksBehind(const ConnectedPlayer* player, std::uint32_t tick) {
  if (player == nullptr) {
    return 0;
  }
  const std::uint32_t shown =
      player->appliedInput.value_or(AppliedInput{}).shownTick;
  if (shown == network::NO_SHOWN_TICK || shown >= tick) {
    return 0;
  }
  return tick - shown;
}

/** @brief A missile or a ship of a player and the ViewDelay it gets. */
struct EntityView {
  engine::Entity entity;
  game::ViewDelay delay;
};

/** @brief Gives each entity of @p views its ViewDelay in @p components. */
void addViews(engine::ComponentRegistry& components,
              const std::vector<EntityView>& views) {
  for (const EntityView& view : views) {
    components.add(view.entity, view.delay);
  }
}

}  // namespace

void giveShooterViews(game::World& world, PlayerRoster& roster,
                      std::uint32_t tick) {
  engine::ComponentRegistry& components = world.components();
  std::vector<EntityView> views;
  components.forEach<game::MissileOwner, game::MissileOwner>(
      [&roster, &views, tick](engine::Entity missile,
                              const game::MissileOwner& owner,
                              const game::MissileOwner&) {
        views.push_back(EntityView{
            .entity = missile,
            .delay = game::ViewDelay{
                .ticks = ticksBehind(roster.findByNumber(owner.playerNumber),
                                     tick)}});
      });
  addViews(components, views);
}

void giveShipViews(game::World& world, PlayerRoster& roster,
                   std::uint32_t tick) {
  engine::ComponentRegistry& components = world.components();
  std::vector<EntityView> views;
  components.forEach<game::EntityType, game::NetworkIdentity>(
      [&roster, &views, tick](engine::Entity ship, const game::EntityType& side,
                              const game::NetworkIdentity& identity) {
        if (side == game::EntityType::Player) {
          views.push_back(EntityView{
              .entity = ship,
              .delay = game::ViewDelay{
                  .ticks = ticksBehind(roster.findByShip(identity.networkId),
                                       tick)}});
        }
      });
  addViews(components, views);
}

void capViewDelays(game::World& world, std::size_t mostTicks) {
  world.components().forEach<game::ViewDelay, game::ViewDelay>(
      [mostTicks](engine::Entity, game::ViewDelay& delay, game::ViewDelay&) {
        delay.ticks = std::min(delay.ticks, mostTicks);
      });
}

}  // namespace rtype::server
