#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include "IInterfaceCanvas.hpp"
#include "KopecksLabel.hpp"

namespace rtype::client {

/** @returns How wide @p label is: the coin, KOPECKS_COIN_GAP, its prefix and
 * its amount in the regular font. */
[[nodiscard]] int kopecksLabelWidth(const KopecksLabel& label,
                                    TextMetrics metrics);

/** @brief Writes the amount of @p label in KOPECKS_COLOR, thousands apart,
 * ending at its top-right corner, and the coin left of it. */
void paintKopecks(IInterfaceCanvas& canvas, const KopecksLabel& label);

/** @brief Paints the balance of a player in its frame, whose top-right
 * corner is @p topRight: the amount, and KOPECKS_CAPTION under it. */
void paintKopecksBalance(IInterfaceCanvas& canvas, sf::Vector2i topRight,
                         std::uint32_t balance);

}  // namespace rtype::client
