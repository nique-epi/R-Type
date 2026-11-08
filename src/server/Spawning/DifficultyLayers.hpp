#pragma once

#include <functional>
#include <map>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "DataFileTable.hpp"

namespace rtype::server {

/**
 * @brief The tables that describe one difficulty, one for each waves file
 * that names it, in the order the files are laid: a key of a later table
 * replaces the same key of the earlier ones.
 */
using DifficultyLayers = std::vector<engine::DataFileTable>;

/** @brief The layers of each difficulty, by its name. */
using LayersByDifficulty = std::map<std::string, DifficultyLayers, std::less<>>;

/**
 * @returns The layers of each difficulty the waves files whose roots are
 * @p roots describe, the roots in the order their files are laid. Each file
 * is checked on its own: the keys of its root and of its difficulties, and
 * the names of its difficulties; the values are read later, once laid.
 * @throws engine::UnknownDataFileKeyException,
 * engine::MissingDataFileKeyException or
 * engine::DataFileTableExpectedException, in the file at fault, when a file
 * is not made of difficulty tables; InvalidDifficultyNameException for a
 * name the protocol cannot carry; TooManyDifficultiesException, at the table
 * of difficulties of the first file that brings their count past
 * network::MAXIMUM_DIFFICULTIES.
 */
[[nodiscard]] LayersByDifficulty difficultyLayersOf(
    std::span<const engine::DataFileTable> roots);

/**
 * @returns The table of @p layers whose @p key wins: the last that holds it;
 * the first when none does, so that a missing key is named at the first table
 * of the difficulty. @p layers is never empty.
 */
[[nodiscard]] const engine::DataFileTable& layerHolding(
    const DifficultyLayers& layers, std::string_view key);

}  // namespace rtype::server
