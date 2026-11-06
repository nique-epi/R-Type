#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <span>
#include <vector>
#include "LabelCandidate.hpp"
#include "LabelSide.hpp"

namespace rtype::client {

/**
 * @returns The side each of @p stars writes its name on, in their order: the
 * first star is served first, so the caller lists the chosen one first. A
 * name takes its right when that box lies between the left and right edges of
 * SECTOR_MAP_AREA, within LABEL_MARGIN, as the mockup bounds it, and overlaps
 * neither @p forbidden nor a name placed before; else its left on the same
 * terms; else it is not written.
 */
[[nodiscard]] std::vector<LabelSide> placeLabels(
    std::span<const LabelCandidate> stars,
    std::span<const sf::IntRect> forbidden);

}  // namespace rtype::client
