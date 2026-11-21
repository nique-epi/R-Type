#pragma once

#include <optional>
#include <string>

namespace rtype::client {

/** @brief The musics the background of the biome shown names, by their asset
 * ids: the music of the level, played in place of the music of a game, and
 * the music of its bosses, played in place of the music of the bosses. Each
 * may be left out. */
struct LevelTracks {
  std::optional<std::string> music = std::nullopt;
  std::optional<std::string> bossMusic = std::nullopt;
};

}  // namespace rtype::client
