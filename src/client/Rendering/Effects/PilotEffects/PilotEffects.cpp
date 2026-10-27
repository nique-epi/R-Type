#include "PilotEffects.hpp"
#include <algorithm>
#include "ComponentRegistry.hpp"
#include "EffectConstants.hpp"
#include "EffectTargets.hpp"
#include "EffectTools.hpp"
#include "Entity.hpp"
#include "FlashGate.hpp"
#include "Invulnerability.hpp"
#include "MissionStartConstants.hpp"
#include "PixelEffects.hpp"
#include "Position.hpp"
#include "Reveal.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @brief Shows every invulnerable ship dithered all along and outlined when
 * @p steady, blinking otherwise: an invulnerability follows the flash
 * preference while it lasts. */
void setSteadiness(engine::ComponentRegistry& components, bool steady) {
  components.forEach<Invulnerability, Invulnerability>(
      [steady](engine::Entity, Invulnerability& invulnerability,
               Invulnerability&) { invulnerability.steady = steady; });
}

}  // namespace

PilotEffects::PilotEffects(EffectTools tools) : tools_(tools) {}

void PilotEffects::advance(engine::Duration elapsed) {
  sinceStart_ += elapsed;
  setSteadiness(tools_.components, !tools_.gate.get().flashes());
}

engine::Duration PilotEffects::sinceStart() const { return sinceStart_; }

void PilotEffects::playArrival(engine::Entity ship) {
  engine::ComponentRegistry& components = tools_.components;
  const game::Position* found = components.get<game::Position>(ship);
  if (found == nullptr) {
    return;
  }
  const game::Position center = *found;
  const bool flashes = tools_.gate.get().flashes();
  const engine::Duration delay =
      std::max(engine::Duration::zero(), PILOT_ARRIVAL_DELAY - sinceStart_);
  components.add(ship, Reveal{.age = -delay,
                              .duration = MATERIALIZE_DURATION,
                              .leadingLine = flashes});
  tools_.pixels.get().speedLines(
      {.shipCenter = center,
       .light = pilotLightOf(pilotColorOf(components, ship)),
       .flashes = flashes,
       .delay = delay,
       .life = MATERIALIZE_DURATION});
}

void PilotEffects::setInvulnerable(engine::Entity ship,
                                   bool invulnerable) const {
  engine::ComponentRegistry& components = tools_.components;
  if (!invulnerable) {
    components.remove<Invulnerability>(ship);
    return;
  }
  if (components.has<Invulnerability>(ship)) {
    return;
  }
  components.add(ship, Invulnerability{.elapsed = engine::Duration::zero(),
                                       .steady = !tools_.gate.get().flashes()});
}

}  // namespace rtype::client
