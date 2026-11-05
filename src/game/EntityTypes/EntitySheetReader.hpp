#pragma once

#include "BehaviourRegistry.hpp"
#include "EntitySheet.hpp"
#include "SheetText.hpp"

namespace rtype::game {

/**
 * @brief Reads the gameplay of one entity sheet: the table `[gameplay]` of
 * @p sheet, as the Entity types section of the architecture page describes it
 * (keys, choices and ranges in EntitySheetConstants.hpp).
 *
 * The root holds exactly the tables `gameplay` and `appearance`; the second is
 * the client's and only checked to be a table. Every key of `[gameplay]` is
 * required but its optional tables (`behaviour`, `health`, `score`, `gun`), and
 * any other key is refused. The table `[gameplay.behaviour]` holds exactly the
 * settings the behaviour of the pilot declares, each in its range, and stands
 * only when that behaviour declares one. A duration of the gun becomes the
 * nearest whole number of ticks, at least 1 for its interval: 333 ms are 20
 * ticks, 1500 ms are 90. Then the sheet must hold together: a player pilots a
 * body of side player and only it, such a body moves as a ship and collides
 * with the box of SHIP_WIDTH by SHIP_HEIGHT, only it moves as a ship, a
 * missile has no gun, and its pilot steers it on its first tick towards the
 * edge PlayfieldExitSystem removes it at: right for a missile of a player,
 * left for one of an enemy. Every error names the file, the line, the column
 * and the dotted key.
 *
 * @param behaviours The behaviours a pilot may name; the pilot of the sheet
 * refers to one of them, so they must outlive the sheet.
 * @returns The sheet, named after @p sheet and numbered 0; the projectile of
 * its gun is still a name, which EntitySheets checks against the other sheets.
 * @throws engine::DataFileSyntaxException when the text is not TOML;
 * engine::MissingDataFileKeyException, UnknownDataFileKeyException,
 * DataFileTableExpectedException, DataFileValueOutOfRangeException,
 * DataFileTextExpectedException or UnknownDataFileChoiceException (a side, a
 * kind of motion or a pilot no one knows) when a key is wrong; and
 * InconsistentEntitySheetException when the sheet does not hold together.
 */
[[nodiscard]] EntitySheet gameplayOf(const SheetText& sheet,
                                     const BehaviourRegistry& behaviours);

}  // namespace rtype::game
