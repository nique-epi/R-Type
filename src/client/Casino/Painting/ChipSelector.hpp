#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <optional>
#include "ChipRow.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @returns Where the chip of @p place of @p row lies, its top-left corner:
 * CHIP_SPACING apart from the first, raised by CHOSEN_CHIP_LIFT when it is
 * the chosen one. */
[[nodiscard]] sf::Vector2i chipTopLeft(const ChipRow& row, std::size_t place);

/** @returns The place of the chip of @p row under @p pixel of the interface,
 * its frame at rest; nothing between the chips. */
[[nodiscard]] std::optional<std::size_t> chipAt(const ChipRow& row,
                                                sf::Vector2i pixel);

/** @returns The place of the chip @p key chooses, if it chooses one. */
[[nodiscard]] std::optional<std::size_t> chipOfKey(sf::Keyboard::Key key);

/** @brief Paints the chips of @p row, the chosen one raised and in corners,
 * each with the keycap of its key under it. */
void paintChipRow(IInterfaceCanvas& canvas, const ChipRow& row);

}  // namespace rtype::client
