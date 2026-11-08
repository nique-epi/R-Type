#pragma once

#include <cstdint>

namespace rtype::server {

/** @brief Where the line editor stands in the sequence an arrow or a function
 * key sends: none, after ESCAPE, or after '[' or 'O'. */
enum class EscapeState : std::uint8_t { None, Started, Sequence };

}  // namespace rtype::server
