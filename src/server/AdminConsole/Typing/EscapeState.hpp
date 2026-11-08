#pragma once

namespace rtype::server {

/** @brief Where the line editor stands in the sequence an arrow or a function
 * key sends: none, after ESCAPE, or after '[' or 'O'. */
enum class EscapeState { None, Started, Sequence };

}  // namespace rtype::server
