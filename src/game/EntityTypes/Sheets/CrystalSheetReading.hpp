#pragma once

#include <optional>
#include <vector>
#include "CrystalDrop.hpp"
#include "DataFileTable.hpp"
#include "DataFileText.hpp"

namespace rtype::game {

/** @returns The weapon the key `crystal` of @p gameplay names, with where it
 * names it; nothing without the key.
 * @throws What engine::textAt() throws. */
[[nodiscard]] std::optional<engine::DataFileText> crystalFrom(
    const engine::DataFileTable& gameplay);

/** @returns The crystals the key `drops` of @p gameplay names, in the order of
 * the file, still unnumbered; none without the key.
 * @throws What engine::textsAt() throws. */
[[nodiscard]] std::vector<CrystalDrop> dropsFrom(
    const engine::DataFileTable& gameplay);

}  // namespace rtype::game
