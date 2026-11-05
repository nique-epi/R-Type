#pragma once

namespace rtype::client::testing {

/** @brief Ids of music files the tests write: the asset library only locates
 * a music, so it stands for any asset without a display or a decoder. */
constexpr const char* STAGE_MUSIC_ID = "music/stage.ogg";
constexpr const char* BOSS_MUSIC_ID = "music/boss.ogg";

/** @brief An id of a texture the tests never write: a texture is read only
 * with a display, so it is only ever missing here. */
constexpr const char* PLAYER_SHIP_ID = "sprites/player_ship.png";

/** @brief What the tests write in a file: enough for a music, which is only
 * located, and unreadable as an image, a font or a sound. */
constexpr const char* NOT_AN_ASSET = "neither an image, a font nor a music";

}  // namespace rtype::client::testing
