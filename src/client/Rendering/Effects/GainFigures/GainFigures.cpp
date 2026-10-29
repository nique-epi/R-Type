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
#include "GainFigure.hpp"
#include "Layer.hpp"
#include "PilotShades.hpp"
#include "PlayerPalette.hpp"
#include "Popup.hpp"
#include "Position.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns How the figure of a gain is written: large for a big catch, in
 * the light shade @p palette gives the only pilot who gained, else STAR_GOLD,
 * outlined in INK so the sky never touches a letter. */
TextStyle figureStyleOf(const GainFigure& figure,
                        const PlayerPalette& palette) {
  return TextStyle{.font = figure.big ? TextFont::Regular : TextFont::Small,
                   .color = figure.pilotColor.has_value()
                                ? pilotLightOf(palette, figure.pilotColor)
                                : STAR_GOLD,
                   .outlined = true,
                   .alignment = TextAlignment::Center};
}

/** @returns The text of the figure of a gain: the number of the only pilot
 * who gained, as the waiting room writes it, then its points; the points
 * alone when no pilot gained alone. */
std::string figureTextOf(const GainFigure& figure) {
  return pilotLabelOf(figure.pilotColor) + GAIN_PREFIX +
         std::to_string(figure.points);
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
  const PilotShades& shades = tools_.shades;
  const engine::Entity entity = tools_.entities.get().create();
  components.add(entity, figure.place);
  components.add(entity, Caption{.text = figureTextOf(figure),
                                 .style = figureStyleOf(figure, shades.palette),
                                 .layer = game::Layer::Popups,
                                 .visible = true});
  components.add(entity, Popup{.originY = figure.place.y,
                               .age = engine::Duration::zero()});
}

}  // namespace rtype::client
