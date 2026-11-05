#pragma once

#include <optional>

namespace rtype::client {

/**
 * @brief The tick the server is at, read now on the clock of this client.
 */
class IServerTicks {
 public:
  virtual ~IServerTicks() = default;

  /** @returns The tick the server is at at this very moment, a fraction of a
   * tick included; nothing while the client does not know the clock of the
   * server. */
  [[nodiscard]] virtual std::optional<double> serverTickNow() const = 0;
};

}  // namespace rtype::client
