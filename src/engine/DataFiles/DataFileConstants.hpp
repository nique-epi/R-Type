#pragma once

#include <string_view>

namespace rtype::engine {

/**
 * @brief Joins the name of a table of a data file and one of its keys in the
 * errors: `difficulty.normal.wave_pause_ms`.
 */
constexpr char DATA_FILE_KEY_SEPARATOR = '.';

/**
 * @brief Surround the index of a table in an array of tables in the errors:
 * `gameplay.hurt_zone.box[1].width`.
 */
constexpr char DATA_FILE_INDEX_OPENING = '[';
constexpr char DATA_FILE_INDEX_CLOSING = ']';

/** @brief The extension of a data file: dataFilesIn() lists the files that end
 * with it. */
constexpr std::string_view DATA_FILE_EXTENSION = ".toml";

}  // namespace rtype::engine
