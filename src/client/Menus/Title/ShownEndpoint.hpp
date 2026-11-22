#pragma once

#include "Endpoint.hpp"
#include "Language.hpp"

namespace rtype::client {

/** @brief The server the ADDRESS and PORT fields of a form show first, and
 * the language their labels are written in. */
struct ShownEndpoint {
  network::Endpoint endpoint;
  Language language = Language::English;
};

}  // namespace rtype::client
