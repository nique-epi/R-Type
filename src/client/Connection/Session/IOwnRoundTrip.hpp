#pragma once

#include <optional>
#include "RoundTripFigures.hpp"
#include "RoundTripSample.hpp"

namespace rtype::client {

/** @brief The round trip between this client and its server, as a debug
 * display reads it: smoothed, and each one as it is measured. */
class IOwnRoundTrip {
 public:
  virtual ~IOwnRoundTrip() = default;

  /** @returns The smoothed round trip and the jitter of this client; nothing
   * without a session, nor before its first round trip. */
  [[nodiscard]] virtual std::optional<network::RoundTripFigures> ownFigures()
      const = 0;

  /** @returns The newest round trip the client measured with its server, as
   * measured; nothing without a session, nor before its first round trip. */
  [[nodiscard]] virtual std::optional<RoundTripSample> newestOwnRoundTrip()
      const = 0;
};

}  // namespace rtype::client
