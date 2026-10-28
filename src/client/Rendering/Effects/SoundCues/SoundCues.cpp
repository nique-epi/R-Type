#include "SoundCues.hpp"
#include <SFML/Graphics/Color.hpp>
#include <optional>
#include <string>
#include <utility>
#include "Anchor.hpp"
#include "BackgroundConstants.hpp"
#include "Caption.hpp"
#include "ComponentRegistry.hpp"
#include "DownedShip.hpp"
#include "EffectConstants.hpp"
#include "EffectTargets.hpp"
#include "EffectTools.hpp"
#include "Entity.hpp"
#include "InterfaceColorConstants.hpp"
#include "InterfaceConstants.hpp"
#include "Layer.hpp"
#include "Lifetime.hpp"
#include "PilotShades.hpp"
#include "Position.hpp"
#include "RenderingConstants.hpp"
#include "Shooter.hpp"
#include "Sprite.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns A cue written in @p color, outlined in INK so the sky never touches
 * a letter, in the Popups layer. */
Caption cueCaption(std::string text, sf::Color color) {
  return Caption{.text = std::move(text),
                 .style = TextStyle{.font = TextFont::Regular,
                                    .color = color,
                                    .outlined = true,
                                    .alignment = TextAlignment::Center},
                 .layer = game::Layer::Popups,
                 .visible = true};
}

/** @returns Where a cue stands over a sprite @p spriteHeight pixels tall
 * centered on @p center: across from its center, the middle of its line half a
 * line plus CUE_GAP pixels above the top of the sprite, since a caption is
 * centered on its place. */
game::Position cuePlaceOver(game::Position center, int spriteHeight) {
  const float rise =
      (static_cast<float>(spriteHeight + TEXT_LINE_HEIGHT) * HALF +
       static_cast<float>(CUE_GAP)) *
      BACKGROUND_PIXEL_SIZE;
  return game::Position{.x = center.x, .y = center.y - rise};
}

}  // namespace

SoundCues::SoundCues(EffectTools tools) : tools_(tools) {}

void SoundCues::showEnemyShot(game::Position missile) const {
  engine::ComponentRegistry& components = tools_.components;
  const std::optional<Shooter> bydo = bydoShooterOf(components, missile);
  if (!bydo.has_value()) {
    return;
  }
  const game::Sprite* sprite = components.get<game::Sprite>(bydo->ship);
  if (sprite == nullptr) {
    return;
  }
  const game::Position place = cuePlaceOver(bydo->center, sprite->frame.height);
  const engine::Entity cue =
      addCue(cueCaption(SHOT_CUE_TEXT, WARNING_ORANGE), place, SHOT_CUE_LIFE);
  components.add(cue, Anchor{.entity = bydo->ship,
                             .offset = {0.0F, place.y - bydo->center.y}});
}

void SoundCues::showPilotHit(engine::Entity ship) const {
  const std::optional<DownedShip> downed =
      downedShipOf(tools_.components, ship);
  if (!downed.has_value()) {
    return;
  }
  const PilotShades& shades = tools_.shades;
  static_cast<void>(addCue(
      cueCaption(pilotLabelOf(downed->pilotColor) + HIT_CUE_TEXT,
                 pilotLightOf(shades.palette, downed->pilotColor)),
      cuePlaceOver(downed->center, downed->sprite.frame.height), HIT_CUE_LIFE));
}

engine::Entity SoundCues::addCue(Caption caption, game::Position place,
                                 engine::Duration life) const {
  engine::ComponentRegistry& components = tools_.components;
  const engine::Entity cue = tools_.entities.get().create();
  components.add(cue, place);
  components.add(cue, std::move(caption));
  components.add(cue, Lifetime{.remaining = life});
  return cue;
}

}  // namespace rtype::client
