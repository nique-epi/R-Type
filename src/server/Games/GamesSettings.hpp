#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <vector>
#include "ServerConstants.hpp"

namespace rtype::server {

/** @brief What the games module is set up with. */
struct GamesSettings {
  /** @brief The seed the random draws of the game start from, unless
   * the configuration sets another; a seed drawn at random when empty. */
  std::optional<std::uint32_t> seed;
  /** @brief The waves file, read when the module starts. */
  std::filesystem::path wavesFile;
  /** @brief The content folders whose entity sheets the games play, the
   * assets first then each resource started (see engine::contentFoldersOf()),
   * read when the module starts, before the waves file that names them. */
  std::vector<std::filesystem::path> contentFolders;
  /** @brief The world states each player of a game is sent per second, unless
   * the configuration sets another rate. */
  std::size_t statesPerSecond{DEFAULT_WORLD_STATES_PER_SECOND};
  /** @brief The ticks a game holds each input before it applies it. */
  std::size_t inputDelayTicks{DEFAULT_INPUT_DELAY_TICKS};
};

}  // namespace rtype::server
