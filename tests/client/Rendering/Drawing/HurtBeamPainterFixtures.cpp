#include "HurtBeamPainterFixtures.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <optional>
#include <vector>
#include "ColoredPixel.hpp"
#include "ComponentRegistry.hpp"
#include "CurrentAction.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "HurtBeam.hpp"
#include "HurtBeamPainter.hpp"
#include "HurtBeamPainterTestConstants.hpp"
#include "HurtWindow.hpp"
#include "HurtZone.hpp"
#include "PaintedBeams.hpp"
#include "RecordingPixelSurface.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

game::HurtZone tyrantBeams() {
  return game::HurtZone{
      .boxes = {},
      .beams = {game::HurtBeam{.offsetX = EYE_OFFSET_X,
                               .offsetY = EYE_OFFSET_Y,
                               .length = TYRANT_BEAM_LENGTH,
                               .thickness = TYRANT_BEAM_THICKNESS,
                               .activeIn = BEAM_STRIKE},
                game::HurtBeam{
                    .offsetX = EYE_OFFSET_X,
                    .offsetY = EYE_OFFSET_Y,
                    .length = TYRANT_BEAM_LENGTH,
                    .thickness = TYRANT_BEAM_THICKNESS,
                    .fromDegrees = SWEEP_FROM_DEGREES,
                    .toDegrees = SWEEP_TO_DEGREES,
                    .activeIn = SWEEP_STRIKE,
                    .window = game::HurtWindow{.from = engine::Duration::zero(),
                                               .until = SWEEP_TIME}}}};
}

std::vector<ColoredPixel> paintedPixels(const game::CurrentAction& shown) {
  engine::EntityRegistry entities;
  engine::ComponentRegistry components{entities};
  const engine::Entity tyrant = entities.create();
  components.add(tyrant, TYRANT_AT_REST);
  components.add(
      tyrant,
      PaintedBeams{.zone = tyrantBeams(), .announced = {}, .shown = shown});
  RecordingPixelSurface surface;
  HurtBeamPainter painter;
  painter.paint(components, surface);
  return surface.plots();
}

std::optional<sf::Color> colorAt(const std::vector<ColoredPixel>& pixels,
                                 sf::Vector2i pixel) {
  const auto found = std::ranges::find_if(pixels.rbegin(), pixels.rend(),
                                          [pixel](const ColoredPixel& plotted) {
                                            return plotted.position == pixel;
                                          });
  if (found == pixels.rend()) {
    return std::nullopt;
  }
  return found->color;
}

}  // namespace rtype::client::testing
