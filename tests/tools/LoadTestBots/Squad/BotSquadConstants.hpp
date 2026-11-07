#pragma once

#include <string_view>

namespace rtype::tools {

/** @brief The nickname of bot n, from 1, is this prefix then n: "BOT7". */
constexpr std::string_view BOT_NICKNAME_PREFIX = "BOT";

/** @brief The folder of the temporary folder of the system where the bots
 * keep the content packs of the servers they join. */
constexpr std::string_view BOT_PACK_CACHE_FOLDER =
    "r-type-load-test-bots-packs";

}  // namespace rtype::tools
