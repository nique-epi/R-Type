#pragma once

#include "SessionArrivals.hpp"

namespace rtype::client {

/** @brief The datagrams of the server this client accepted and missed, as a
 * screen reads them. */
class ISessionArrivals {
 public:
  virtual ~ISessionArrivals() = default;

  /** @returns What was counted since the client started. */
  [[nodiscard]] virtual SessionArrivals arrivals() const = 0;
};

}  // namespace rtype::client
