#pragma once

#include <cstdint>
#include "EntitySheets.hpp"

namespace rtype::server {

/**
 * @returns SipHash-2-4, under ENTITY_SHEETS_FINGERPRINT_KEY, of the sheets of
 * @p types in the order of their names: for each, its name, SHEET_TEXT_END,
 * the text it was read from, SHEET_TEXT_END. A set of sheets has the same
 * fingerprint on every platform, and any change of a name or of a text, a
 * comment or a line ending included, gives another one, unless it hits a
 * collision: one chance in 2^64 for a change made by chance.
 */
[[nodiscard]] std::uint64_t entitySheetsFingerprint(
    const game::EntitySheets& types);

}  // namespace rtype::server
