#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>

namespace rtype::server {

/** @brief What the games of the server are built from. */
struct GameListSettings {
  /** @brief The seed the random draws of the games start from; nothing to
   * draw one at random when the games start. */
  std::optional<std::uint32_t> seed;
  /** @brief The waves file the games read their DEFAULT_DIFFICULTY from, when
   * they start. */
  std::filesystem::path waveFile;
};

}  // namespace rtype::server
