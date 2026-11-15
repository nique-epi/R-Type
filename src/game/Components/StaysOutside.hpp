#pragma once

namespace rtype::game {

/** @brief Marker of an enemy that leaving the playfield never removes: the
 * Bydo Tyrant, whose charge takes it past the left edge and back. Declared by
 * `[gameplay.stays_outside]`; PlayfieldExitSystem reads it. */
struct StaysOutside {};

}  // namespace rtype::game
