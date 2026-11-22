#pragma once

#include <optional>
#include "Endpoint.hpp"
#include "Translation.hpp"

namespace rtype::client {

/** @brief What the ADDRESS and PORT fields of a form read. */
struct EndpointReading {
  /** @brief The server the two fields name, when both read. */
  std::optional<network::Endpoint> endpoint = std::nullopt;
  /** @brief The status line naming the field to fix; nothing when both
   * read. */
  std::optional<Translation> problem = std::nullopt;
};

}  // namespace rtype::client
