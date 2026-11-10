#pragma once

#include <cstdint>

namespace rtype::engine {

/** @brief How a resumed Luau thread gave control back: its function returned,
 * or it yielded and waits to be resumed. */
enum class LuauResumeResult : std::uint8_t { Returned, Yielded };

}  // namespace rtype::engine
