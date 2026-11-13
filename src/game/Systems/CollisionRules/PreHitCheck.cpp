#include "PreHitCheck.hpp"
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "IncomingHit.hpp"
#include "MissileHit.hpp"
#include "NetworkIdentity.hpp"
#include "Pilot.hpp"
#include "Position.hpp"

namespace rtype::game {

namespace {

/** @returns The center of @p entity in @p components; the origin when it has
 * no Position. */
Position centerOf(engine::ComponentRegistry& components,
                  engine::Entity entity) {
  const auto* const position = components.get<Position>(entity);
  return position == nullptr ? Position{} : *position;
}

/** @returns What the target of @p hit is told of its missile. */
IncomingHit incomingHitOf(engine::ComponentRegistry& components,
                          const MissileHit& hit) {
  const auto* const identity = components.get<NetworkIdentity>(hit.missile);
  const Position missile = centerOf(components, hit.missile);
  const Position target = centerOf(components, hit.target);
  return IncomingHit{
      .missileNetworkId = identity == nullptr ? 0 : identity->networkId,
      .offset = Position{.x = missile.x - target.x, .y = missile.y - target.y}};
}

}  // namespace

bool targetLetsHit(engine::ComponentRegistry& components,
                   const MissileHit& hit) {
  const auto* const pilot = components.get<Pilot>(hit.target);
  if (pilot == nullptr || pilot->behaviourState == nullptr) {
    return true;
  }
  return pilot->behaviourState->letsHit(incomingHitOf(components, hit));
}

}  // namespace rtype::game
