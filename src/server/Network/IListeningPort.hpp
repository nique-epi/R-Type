#pragma once

#include <cstdint>

namespace rtype::server {

/**
 * @brief The UDP port the server listens on, for the modules that name
 * something after it, such as the file of its leaderboard.
 */
class IListeningPort {
 public:
  virtual ~IListeningPort() = default;

  /**
   * @returns The port the server listens on once the network module started,
   * the one the system chose when the configuration gives 0; before that, the
   * port the configuration gives. The modules listed after the network module
   * read it from their start().
   */
  [[nodiscard]] virtual std::uint16_t listeningPort() const = 0;
};

}  // namespace rtype::server
