#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <string_view>
#include "CasinoBalances.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @brief Paints the four edges of @p area, one pixel wide, in @p color. */
void paintEdges(IInterfaceCanvas& canvas, sf::IntRect area, sf::Color color);

/** @returns @p area, @p inset pixels smaller on every side. */
[[nodiscard]] sf::IntRect insetOf(sf::IntRect area, int inset);

/** @brief Paints a panel of the casino over @p area: the panel of the client,
 * edged twice in gold, its corners cut like a gaming plaque. */
void paintGoldPanel(IInterfaceCanvas& canvas, sf::IntRect area);

/** @brief Paints the tag that names a panel, its top-left corner at
 * @p topLeft: a box edged in gold, as wide as @p name needs. */
void paintNameTag(IInterfaceCanvas& canvas, sf::Vector2i topLeft,
                  std::string_view name);

/** @brief Paints the two balances of the casino in CASINO_BALANCES_AREA: the
 * kopecks in gold, the chips in white, each over its name. */
void paintCasinoBalances(IInterfaceCanvas& canvas, CasinoBalances balances);

/** @brief Paints the panel of the butler, CASINO_SPEECH_AREA, and his name on
 * its tag; what he says is written in it by his SpokenLine. */
void paintButlerPanel(IInterfaceCanvas& canvas);

}  // namespace rtype::client
