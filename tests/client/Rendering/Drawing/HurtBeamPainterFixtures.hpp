#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <optional>
#include <vector>
#include "ColoredPixel.hpp"
#include "CurrentAction.hpp"
#include "HurtZone.hpp"

namespace rtype::client::testing {

/** @returns The two beams of the hurt zone of the Bydo Tyrant: its straight
 * beam of attack 2 and its sweep of attack 15. */
[[nodiscard]] game::HurtZone tyrantBeams();

/** @returns The pixels a HurtBeamPainter plots for the Tyrant at rest, its
 * PaintedBeams showing @p shown. */
[[nodiscard]] std::vector<ColoredPixel> paintedPixels(
    const game::CurrentAction& shown);

/** @returns The color plotted at @p pixel among @p pixels, the last one when
 * several are; nothing when none is. */
[[nodiscard]] std::optional<sf::Color> colorAt(
    const std::vector<ColoredPixel>& pixels, sf::Vector2i pixel);

}  // namespace rtype::client::testing
