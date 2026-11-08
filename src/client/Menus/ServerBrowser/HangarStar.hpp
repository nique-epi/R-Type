#pragma once

#include <SFML/System/Vector2.hpp>
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @returns Whether @p pixel of the interface picks the hangar star: within
 * STAR_PICK_REACH of HANGAR_STAR_POINT, as a server star. */
[[nodiscard]] bool hangarStarAt(sf::Vector2i pixel);

/** @brief Paints the hangar star and its name; with @p shown, the corners of
 * a chosen star and its bubble too. */
void paintHangarStar(IInterfaceCanvas& canvas, bool shown);

}  // namespace rtype::client
