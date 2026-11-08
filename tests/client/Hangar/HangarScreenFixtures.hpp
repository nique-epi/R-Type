#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <cstddef>
#include <optional>
#include "RecordingInterfaceCanvas.hpp"

namespace rtype::client::testing {

/** @returns How many drawings of the item sheet @p painting holds at the
 * scale of the ship of the hangar, of @p source only when it is given. */
[[nodiscard]] std::size_t lookImagesAtShipScale(
    const InterfaceCanvasRecord& painting, std::optional<sf::IntRect> source);

}  // namespace rtype::client::testing
