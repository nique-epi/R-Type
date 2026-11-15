#pragma once

#include <filesystem>
#include <vector>
#include "LdtkPlacement.hpp"

namespace rtype::game {

/**
 * @returns The enemies the levels of the LDtk project @p file place on their
 * layer of entities MOBS_LAYER, sorted by the x of their center, each level
 * laid after the one before it in the file, the first starting at 0. The
 * identifier of an entity names its sheet in lowercase, its words joined by
 * underscores (`BydoViolet` places a `bydo_violet`); its box is converted at
 * LDTK_PIXEL units a pixel, and its center is read from its pivot. Fields of
 * a kind the game reads (see LdtkFieldValue) come with it; a field whose
 * value is null is left out.
 * @throws engine::UnreadableDataFileException when @p file cannot be read;
 * InvalidLdtkFileException, naming the file and the reason, when it is not a
 * JSON object, a member is missing or of the wrong kind, a level is saved in a
 * separate file, has no MOBS_LAYER layer of entities, an entity has a field of
 * another kind, or its identifier is no sheet name.
 */
[[nodiscard]] std::vector<LdtkPlacement> ldtkPlacementsOf(
    const std::filesystem::path& file);

}  // namespace rtype::game
