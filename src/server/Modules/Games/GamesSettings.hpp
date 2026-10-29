#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include "ServerConstants.hpp"

namespace rtype::server {

/** @brief What the games module is set up with. */
struct GamesSettings {
  /** @brief The seed the random draws of the first game start from; a seed
   * drawn at random when empty. */
  std::optional<std::uint32_t> seed;
  /** @brief The waves file, read when the module starts. */
  std::filesystem::path wavesFile;
  /** @brief How many games the module creates when it starts; at least 1. */
  std::size_t gameCount{DEFAULT_HOSTED_GAME_COUNT};
  /** @brief The world states each player of a game is sent per second. */
  std::size_t statesPerSecond{DEFAULT_WORLD_STATES_PER_SECOND};
};

}  // namespace rtype::server
