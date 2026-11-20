#pragma once

#include <filesystem>
#include <optional>
#include "BiomeGround.hpp"

namespace rtype::game {

/**
 * @returns The ground the levels of the LDtk project @p file give in their
 * fields FLOOR_FIELD and CEILING_FIELD, the levels laid end to end in the
 * order of the file; nothing when its first level has no FLOOR_FIELD, a
 * project exported before the ground existed.
 * @throws engine::UnreadableDataFileException when @p file cannot be read;
 * InvalidLdtkFileException, naming the file and the reason, when it is not a
 * JSON object, a member is missing or of the wrong kind, a level lacks one of
 * the two fields while the first has them, or a field does not hold a whole
 * number for each pixel column of its level.
 */
[[nodiscard]] std::optional<BiomeGround> ldtkGroundOf(
    const std::filesystem::path& file);

}  // namespace rtype::game
