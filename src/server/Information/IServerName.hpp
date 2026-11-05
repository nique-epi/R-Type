#pragma once

#include <string>

namespace rtype::server {

/**
 * @brief The name players read for this server, the server-name setting, for
 * the modules that announce the server: the information module answers it to
 * whoever asks, the master module registers it.
 */
class IServerName {
 public:
  virtual ~IServerName() = default;

  /**
   * @returns The name the configuration gives the server: final once the
   * configuration was read, that is when the modules start.
   */
  [[nodiscard]] virtual std::string serverName() const = 0;
};

}  // namespace rtype::server
