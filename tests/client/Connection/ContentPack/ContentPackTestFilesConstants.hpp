#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>

namespace rtype::client::testing {

/** @brief The files of a pack of one Bydo: its sheet and its sprite sheet. */
constexpr std::string_view LANCER_SHEET_PATH = "entities/lancer.toml";
constexpr std::string_view LANCER_SHEET =
    "[gameplay]\nside = \"enemy\"\npilot = \"lancer_charge\"\n"
    "[gameplay.hitbox]\nwidth = 96\nheight = 96\n"
    "[appearance]\nsheet = \"sprites/mobs/lancer.png\"\n"
    "[appearance.idle]\nleft = 0\ntop = 0\nwidth = 24\nheight = 16\n"
    "frames = 1\nspacing = 0\nframe_ms = 100\n";
constexpr std::string_view LANCER_TYPE = "lancer";

/** @brief A sheet whose sprite sheet lies outside its pack. */
constexpr std::string_view SHEET_NAMING_A_FILE_OUTSIDE =
    "[gameplay]\nside = \"enemy\"\n"
    "[gameplay.hitbox]\nwidth = 96\nheight = 96\n"
    "[appearance]\nsheet = \"../../secret.png\"\n"
    "[appearance.idle]\nleft = 0\ntop = 0\nwidth = 24\nheight = 16\n"
    "frames = 1\nspacing = 0\nframe_ms = 100\n";
constexpr std::string_view LANCER_SPRITES_PATH = "sprites/mobs/lancer.png";
constexpr std::string_view LANCER_PIXELS = "\x89PNG\r\n\x1a\nlancer";

/** @brief What a kept pack is named after its fingerprint, and the types a
 * pack of one sheet gives. */
constexpr std::string_view KEPT_PACK_EXTENSION = ".pack";
constexpr std::size_t ONE_TYPE = 1;

/** @brief The packs a client keeps, as the page of the protocol states it. */
constexpr std::size_t KEPT_PACKS = 8;

/** @brief A pack is written once a second apart from the one before, so the
 * disk orders them whatever the precision of its times. */
constexpr std::chrono::seconds TIME_BETWEEN_TWO_PACKS{1};

/** @brief A byte a test changes in a kept pack, and its new value. */
constexpr std::size_t CHANGED_BYTE = 5;
constexpr char OTHER_VALUE = '#';

}  // namespace rtype::client::testing
