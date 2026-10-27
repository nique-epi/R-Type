#include "SpriteEffectSystem.hpp"
#include "Anchor.hpp"
#include "BackgroundConstants.hpp"
#include "Caption.hpp"
#include "ComponentRegistry.hpp"
#include "Easing.hpp"
#include "Entity.hpp"
#include "Invulnerability.hpp"
#include "Lifetime.hpp"
#include "Nudge.hpp"
#include "Popup.hpp"
#include "PopupConstants.hpp"
#include "Position.hpp"
#include "Reveal.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns How far a figure of that age has risen above where it started, in
 * logical units: nothing before 0, POPUP_RISE_HEIGHT pixels of the canvas
 * from POPUP_RISE on. */
float riseOf(engine::Duration age) {
  return easeOutQuad(shareOf(age, POPUP_RISE)) *
         static_cast<float>(POPUP_RISE_HEIGHT) * BACKGROUND_PIXEL_SIZE;
}

/** @returns Whether a figure shows: not before 0, all along its rise and its
 * hold, then on every even blink step, or all along when it is steady. */
bool isPopupShown(const Popup& popup) {
  if (popup.age < engine::Duration::zero()) {
    return false;
  }
  const engine::Duration intoBlinks = popup.age - POPUP_RISE - POPUP_HOLD;
  return popup.steady || intoBlinks < engine::Duration::zero() ||
         (intoBlinks / POPUP_BLINK_STEP) % 2 == 0;
}

}  // namespace

void SpriteEffectSystem::update(engine::ComponentRegistry& components,
                                engine::Duration elapsed) {
  spentNudges_.clear();
  spentReveals_.clear();
  advanceNudges(components, elapsed);
  advanceLifetimes(components, elapsed);
  followAnchors(components);
  advancePopups(components, elapsed);
  advanceReveals(components, elapsed);
  advanceInvulnerabilities(components, elapsed);
  removeSpent(components);
}

void SpriteEffectSystem::advanceNudges(engine::ComponentRegistry& components,
                                       engine::Duration elapsed) {
  components.forEach<Nudge, Nudge>(
      [this, elapsed](engine::Entity entity, Nudge& nudge, Nudge&) {
        nudge.elapsed += elapsed;
        if (nudge.elapsed >= nudge.duration) {
          spentNudges_.push_back(entity);
        }
      });
}

void SpriteEffectSystem::advanceLifetimes(engine::ComponentRegistry& components,
                                          engine::Duration elapsed) {
  components.forEach<Lifetime, Lifetime>(
      [&components, elapsed](engine::Entity entity, Lifetime& lifetime,
                             Lifetime&) {
        lifetime.remaining -= elapsed;
        if (lifetime.remaining <= engine::Duration::zero()) {
          components.destroy(entity);
        }
      });
}

void SpriteEffectSystem::followAnchors(engine::ComponentRegistry& components) {
  components.forEach<Anchor, game::Position>(
      [&components](engine::Entity, Anchor& anchor, game::Position& position) {
        const game::Position* followed =
            components.get<game::Position>(anchor.entity);
        if (followed != nullptr) {
          position = game::Position{.x = followed->x + anchor.offset.x,
                                    .y = followed->y + anchor.offset.y};
        }
      });
}

void SpriteEffectSystem::advancePopups(engine::ComponentRegistry& components,
                                       engine::Duration elapsed) {
  components.forEach<Popup, game::Position>(
      [&components, elapsed](engine::Entity entity, Popup& popup,
                             game::Position& position) {
        popup.age += elapsed;
        if (popup.age >= POPUP_LIFE) {
          components.destroy(entity);
          return;
        }
        position.y = popup.originY - riseOf(popup.age);
        if (auto* caption = components.get<Caption>(entity);
            caption != nullptr) {
          caption->visible = isPopupShown(popup);
        }
      });
}

void SpriteEffectSystem::advanceReveals(engine::ComponentRegistry& components,
                                        engine::Duration elapsed) {
  components.forEach<Reveal, Reveal>(
      [this, elapsed](engine::Entity entity, Reveal& reveal, Reveal&) {
        reveal.age += elapsed;
        if (reveal.age >= reveal.duration) {
          spentReveals_.push_back(entity);
        }
      });
}

void SpriteEffectSystem::advanceInvulnerabilities(
    engine::ComponentRegistry& components, engine::Duration elapsed) {
  components.forEach<Invulnerability, Invulnerability>(
      [elapsed](engine::Entity, Invulnerability& invulnerability,
                Invulnerability&) { invulnerability.elapsed += elapsed; });
}

void SpriteEffectSystem::removeSpent(engine::ComponentRegistry& components) {
  for (const engine::Entity entity : spentNudges_) {
    components.remove<Nudge>(entity);
  }
  for (const engine::Entity entity : spentReveals_) {
    components.remove<Reveal>(entity);
  }
}

}  // namespace rtype::client
