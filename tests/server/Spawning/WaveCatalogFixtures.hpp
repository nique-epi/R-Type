#pragma once

#include <string>
#include <string_view>
#include "WaveCatalog.hpp"

namespace rtype::server::testing {

/**
 * @returns A waves file holding calm alone: CALM_HEADER on line 1, then the
 * lines of CALM_VALUES.
 */
[[nodiscard]] std::string calmFile();

/**
 * @returns calmFile() with @p value for @p key; when calm has no such key, the
 * line `key = value` comes last, on line 10.
 */
[[nodiscard]] std::string calmFileWith(std::string_view key,
                                       std::string_view value);

/** @returns calmFile() without the line of @p key. */
[[nodiscard]] std::string calmFileWithout(std::string_view key);

/**
 * @returns The message of the error reading @p text throws, an exception of
 * the server or of the engine; an empty text, after a failure, when it throws
 * none.
 */
[[nodiscard]] std::string errorReading(std::string_view text);

/** @returns The waves file the game ships, read where the server finds it in
 * the assets folder of the sources. */
[[nodiscard]] WaveCatalog shippedCatalog();

/** @returns @p text with each line ending in CRLF, as a Windows checkout that
 * converts line endings writes it. */
[[nodiscard]] std::string withCrlfLineEndings(std::string_view text);

}  // namespace rtype::server::testing
