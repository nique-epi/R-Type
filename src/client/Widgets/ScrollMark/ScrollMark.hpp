#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <optional>
#include "IInterfaceCanvas.hpp"
#include "RowWindow.hpp"

namespace rtype::client {

/** @returns Where the thumb lies on the vertical @p track: as long as the
 * share of the rows @p rows shows, as far down as its first row is among
 * those it may start from; nothing when every row shows. */
[[nodiscard]] std::optional<sf::IntRect> scrollThumbOf(sf::IntRect track,
                                                       RowWindow rows);

/** @brief Paints @p track and its thumb; nothing when every row shows. */
void paintScrollMark(IInterfaceCanvas& canvas, sf::IntRect track,
                     RowWindow rows);

}  // namespace rtype::client
