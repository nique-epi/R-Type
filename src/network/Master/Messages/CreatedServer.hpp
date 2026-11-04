#pragma once

#include <cstdint>
#include <string>

namespace rtype::network {

/**
 * @brief What the master answers when it created a game server: its id, its
 * name and its key, which the master never shows again.
 */
struct CreatedServer {
  std::int64_t id{};
  std::string name;
  /** @brief The key the host writes in server.cfg, SERVER_KEY_SIZE long. */
  std::string key;

  bool operator==(const CreatedServer&) const = default;
};

}  // namespace rtype::network
