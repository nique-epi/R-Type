#pragma once

#include <cstdint>

namespace rtype::client {

/**
 * @brief The check box of a button: none for an ordinary button; empty, or
 * filled for the current one, for a button that is one choice among others.
 */
enum class CheckMark : std::uint8_t {
  None,
  Unchecked,
  Checked,
};

}  // namespace rtype::client
