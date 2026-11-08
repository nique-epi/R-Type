#pragma once

#include <string_view>

namespace rtype::server::testing {

/** @brief The file the tests of the bans of the supervisor open in their
 * folder. */
constexpr std::string_view BAN_BOOK_FILE_NAME = "ban-book.db";

/** @brief Removes the table of the bans from under an open book, so its next
 * write is refused. */
constexpr std::string_view DROP_BANS_STATEMENT = "DROP TABLE bans";

}  // namespace rtype::server::testing
