#pragma once

#include <cstddef>

namespace rtype::server {

/** @brief The outputs of a child the launcher relays: its standard output and
 * its standard error. */
constexpr std::size_t CHILD_OUTPUTS = 2;

/** @brief The bytes one read of an output of a child takes. */
constexpr std::size_t CHILD_OUTPUT_READ_SIZE = 4096;

/** @brief The longest line of a child relayed whole: a longer one is cut into
 * lines of this length, so a child that never ends a line cannot make the
 * supervisor keep its output. */
constexpr std::size_t MAXIMUM_CHILD_LINE_LENGTH = 4096;

/** @brief The end of a line a child writes, and the character a Windows
 * program puts before it. */
constexpr char CHILD_LINE_END = '\n';
constexpr char CHILD_CARRIAGE_RETURN = '\r';

}  // namespace rtype::server
