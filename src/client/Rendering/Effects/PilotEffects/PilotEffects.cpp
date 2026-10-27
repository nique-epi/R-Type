#include "PilotEffects.hpp"
#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <optional>
#include "ComponentRegistry.hpp"
#include "EffectConstants.hpp"
#include "EffectTargets.hpp"
#include "EffectTools.hpp"
#include "Entity.hpp"
#include "FlashGate.hpp"
#include "Invulnerability.hpp"
#include "MissionStartConstants.hpp"
#include "PilotShades.hpp"
#include "PixelEffects.hpp"
#include "PlayerPalette.hpp"
#include "Position.hpp"
#include "Reveal.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The outline of the steady invulnerability of a ship: the light
 * shade @p palette gives its pilot, none for a pilot not known. */
std::optional<sf::Color> outlineOf(const PlayerPalette& palette,
                                   std::optional<int> pilotColor) {
  if (!pilotColor.has_value()) {
    return std::nullopt;
  }
  return pilotLightOf(palette, pilotColor);
}

/** @returns The invulnerability a ship starts: steady while effects may not
 * flash, then outlined in the light shade of its pilot. */
Invulnerability invulnerabilityOf(const EffectTools& tools,
                                  engine::Entity ship) {
  const PilotShades& shades = tools.shades;
  return Invulnerability{
      .elapsed = engine::Duration::zero(),
      .steady = !tools.gate.get().flashes(),
      .outline =
          outlineOf(shades.palette, pilotColorOf(tools.components, ship))};
}

}  // namespace

PilotEffects::PilotEffects(EffectTools tools) : tools_(tools) {}

void PilotEffects::advance(engine::Duration elapsed) { sinceStart_ += elapsed; }

engine::Duration PilotEffects::sinceStart() const { return sinceStart_; }

void PilotEffects::playArrival(engine::Entity ship) {
  engine::ComponentRegistry& components = tools_.components;
  const PilotShades& shades = tools_.shades;
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
       .light = pilotLightOf(shades.palette, pilotColorOf(components, ship)),
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
  components.add(ship, invulnerabilityOf(tools_, ship));
}

}  // namespace rtype::client
