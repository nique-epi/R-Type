#pragma once

#include <optional>
#include <string_view>
#include "Endpoint.hpp"

namespace rtype::client {

/** @brief What the ADDRESS and PORT fields of a form read. */
struct EndpointReading {
  /** @brief The server the two fields name, when both read. */
  std::optional<network::Endpoint> endpoint = std::nullopt;
  /** @brief The status line naming the field to fix; empty when both read. */
  std::string_view problem;
};

}  // namespace rtype::client
