#pragma once

#include <string>

namespace rtype::server {

/** @brief Why the server was asked to stop. */
struct StopReason {
  /** @brief Whether a part of the server failed, instead of the operator or
   * the system asking for the stop. */
  bool failed{false};
  /** @brief What failed, as the error says it; empty when nothing failed. */
  std::string text;
};

}  // namespace rtype::server
