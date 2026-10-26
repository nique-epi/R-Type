#pragma once

#include <SFML/Graphics/Color.hpp>
#include <span>
#include "FlashPreference.hpp"

namespace rtype::client {

/** @returns The color of @p ramp that a share of life reaches: the first for
 * 0, the last one for the end and past it. @p ramp is never empty. */
[[nodiscard]] sf::Color colorAt(std::span<const sf::Color> ramp, float share);

/** @returns @p ramp as the preference shows it: whole with
 * FlashPreference::Full, without its first color, its flash, with REDUCE
 * FLASHES; an empty ramp stays empty. */
[[nodiscard]] std::span<const sf::Color> rampFor(
    std::span<const sf::Color> ramp, FlashPreference preference);

}  // namespace rtype::client
