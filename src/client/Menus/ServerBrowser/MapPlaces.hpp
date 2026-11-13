#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <optional>
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @returns The place of MAP_PLACES that @p pixel of the interface picks:
 * the one within STAR_PICK_REACH of its point, as a server star; none
 * elsewhere. */
[[nodiscard]] std::optional<std::size_t> mapPlaceAt(sf::Vector2i pixel);

/** @returns The place of MAP_PLACES that @p key opens, if any. */
[[nodiscard]] std::optional<std::size_t> mapPlaceOpenedBy(
    sf::Keyboard::Key key);

/** @brief Paints every place of MAP_PLACES, its glyph centered on its point,
 * and its name; the place @p shown also gets the corners of a chosen star
 * and its bubble. */
void paintMapPlaces(IInterfaceCanvas& canvas, std::optional<std::size_t> shown);

}  // namespace rtype::client
