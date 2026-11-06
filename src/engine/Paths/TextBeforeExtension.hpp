#pragma once

#include <filesystem>
#include <string_view>

namespace rtype::engine {

/**
 * @returns @p file with @p text written between its name and its extension,
 * in the same folder: `logs/traffic.csv` and `_game_3` give
 * `logs/traffic_game_3.csv`.
 */
[[nodiscard]] std::filesystem::path withTextBeforeExtension(
    const std::filesystem::path& file, std::string_view text);

}  // namespace rtype::engine
