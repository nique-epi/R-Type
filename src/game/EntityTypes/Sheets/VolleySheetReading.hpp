#pragma once

#include <vector>
#include "DataFileTable.hpp"
#include "VolleySheet.hpp"

namespace rtype::game {

/**
 * @returns The volleys of the tables `[[gameplay.volley]]` of @p gameplay, in
 * the order of the file; none without them. Their bodies are still names,
 * which EntitySheets checks against the other sheets.
 * @throws What the engine::…At() readers throw for a key, and
 * InconsistentEntitySheetException when a gap leaves no body.
 */
[[nodiscard]] std::vector<VolleySheet> volleysFrom(
    const engine::DataFileTable& gameplay);

}  // namespace rtype::game
