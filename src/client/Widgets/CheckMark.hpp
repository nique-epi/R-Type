#pragma once

#include <cstdint>

namespace rtype::client {

/**
 * @brief The check box of a choice among others: empty, or filled for the
 * current choice; dashed for a choice that cannot be taken, so it reads
 * without its color.
 */
enum class CheckMark : std::uint8_t {
  Unchecked,
  Checked,
  Unavailable,
};

}  // namespace rtype::client
