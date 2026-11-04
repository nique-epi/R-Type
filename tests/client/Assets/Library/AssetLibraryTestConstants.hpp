#pragma once

namespace rtype::client::testing {

/** @brief The assets folder, inside a folder that also holds a file outside
 * it, which an escaping id names. */
constexpr const char* ASSETS_FOLDER = "assets";
constexpr const char* OUTSIDE_FILE_NAME = "outside.ogg";
constexpr const char* ESCAPING_MUSIC_ID = "../outside.ogg";

/** @brief A font never loaded, and a file of the folder that holds no
 * font. */
constexpr const char* HUD_FONT_ID = "fonts/hud.ttf";
constexpr const char* CORRUPT_ID = "corrupt/asset.bin";

}  // namespace rtype::client::testing
