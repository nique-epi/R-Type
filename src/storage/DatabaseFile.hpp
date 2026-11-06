#pragma once

#include <SQLiteCpp/Database.h>
#include <span>
#include <string>
#include <string_view>

namespace rtype::storage {

/** @returns The flags that open a file for writing and create it when it does
 * not exist; SQLiteCpp declares each as a signed value. */
[[nodiscard]] int readWriteOrCreate();

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
 * Several programs may open the same file at once, as the games of one
 * supervisor do: each waits up to DATABASE_BUSY_TIMEOUT_MILLISECONDS for the
 * others, and a migration one of them applied is never applied again.
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
