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
   * @returns The port the configuration gives the server: final once the
   * configuration was read, that is when the modules start. 0 when the system
   * picks one.
   */
  [[nodiscard]] virtual std::uint16_t listeningPort() const = 0;
};

}  // namespace rtype::server
