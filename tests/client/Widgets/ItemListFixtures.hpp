#pragma once

#include <optional>
#include <string>
#include <vector>
#include "ItemRow.hpp"
#include "RecordingInterfaceCanvas.hpp"

/** @returns A row of two cells: its rank, then its name. */
[[nodiscard]] rtype::client::ItemRow rowNamed(const std::string& name);

/** @returns What a list of LIST_LAYOUT paints with these rows. */
[[nodiscard]] InterfaceCanvasRecord paintedList(
    std::vector<rtype::client::ItemRow> rows,
    std::optional<rtype::client::ItemRow> pinned = std::nullopt);

/** @returns What a list of LIST_LAYOUT paints with one row and this note. */
[[nodiscard]] InterfaceCanvasRecord paintedListWithNote(
    const std::string& note);
