#include "GainFigures.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include "BackgroundColorConstants.hpp"
#include "Caption.hpp"
#include "ComponentRegistry.hpp"
#include "EffectConstants.hpp"
#include "EffectTargets.hpp"
#include "EffectTools.hpp"
#include "Entity.hpp"
#include "FlashGate.hpp"
#include "GainFigure.hpp"
#include "Layer.hpp"
#include "Popup.hpp"
#include "Position.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns How the figure of a gain is written: large for a big catch, in
 * the light shade of the only pilot who gained, else STAR_GOLD, outlined in
 * INK so the sky never touches a letter. */
TextStyle figureStyleOf(const GainFigure& figure) {
  return TextStyle{.font = figure.big ? TextFont::Regular : TextFont::Small,
                   .color = figure.pilotColor.has_value()
                                ? pilotLightOf(figure.pilotColor)
                                : STAR_GOLD,
                   .outlined = true,
                   .alignment = TextAlignment::Center};
}

}  // namespace

GainFigures::GainFigures(EffectTools tools) : tools_(tools) {}

void GainFigures::noteKill(game::Position place, bool big) {
  attribution_.noteKill(place, big);
  showGain();
}

void GainFigures::noteScore(int pilotColor, std::uint32_t score) {
  attribution_.noteScore(pilotColor, score);
  showGain();
}

void GainFigures::advance(engine::Duration elapsed) {
  attribution_.advance(elapsed);
}

void GainFigures::showGain() {
  if (const std::optional<GainFigure> figure = attribution_.takeFigure();
      figure.has_value()) {
    addGainFigure(*figure);
  }
}

void GainFigures::addGainFigure(const GainFigure& figure) const {
  engine::ComponentRegistry& components = tools_.components;
  const engine::Entity entity = tools_.entities.get().create();
  components.add(entity, figure.place);
  components.add(entity,
                 Caption{.text = GAIN_PREFIX + std::to_string(figure.points),
                         .style = figureStyleOf(figure),
                         .layer = game::Layer::Popups,
                         .visible = true});
  components.add(entity, Popup{.originY = figure.place.y,
                               .age = engine::Duration::zero(),
                               .steady = !tools_.gate.get().flashes()});
}

}  // namespace rtype::client
