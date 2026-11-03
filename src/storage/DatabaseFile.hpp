#pragma once

#include <SQLiteCpp/Database.h>
#include <span>
#include <string>
#include <string_view>

namespace rtype::storage {

/**
 * @brief Opens @p file, a path written in UTF-8, creating it when it does not
 * exist, then applies those of @p migrations it does not hold yet;
 * migrations[0] is version 1. Every connection of the project opens here, so
 * its settings are written once.
 *
 * The connection uses a write-ahead log with synchronous FULL, which the SQLite
 * documentation calls atomic, consistent, isolated and durable, provided the
 * disk honours a flush. A process killed loses no committed transaction. An
 * operating system crash or a power cut rest on that documentation: neither is
 * tested here. WAL needs the file on the machine that runs the program.
 *
 * Each migration commits together with its number, so a crash between two of
 * them leaves a file the next start carries on from.
 *
 * Use the connection from one thread at a time.
 *
 * @throws UnopenableDatabaseException when SQLite refuses the file or a
 * migration; the migration that failed leaves nothing behind.
 * @throws JournalModeRefusedException when the file cannot use WAL.
 * @throws NewerDatabaseException when the file holds more migrations than
 * @p migrations lists.
 */
[[nodiscard]] SQLite::Database openDatabaseFile(
    const std::string& file, std::span<const std::string_view> migrations);

}  // namespace rtype::storage
