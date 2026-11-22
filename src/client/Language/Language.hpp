#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief The language the client writes its texts in. */
enum class Language : std::uint8_t {
  English,
  French,
};

}  // namespace rtype::client
