#pragma once

#include <string>
#include <string_view>

namespace rtype::server::testing {

/**
 * @returns A waves file holding calm alone: CALM_HEADER on line 1, then the
 * lines of CALM_VALUES.
 */
[[nodiscard]] std::string calmFile();

/**
 * @returns calmFile() with @p value for @p key; when calm has no such key, the
 * line `key = value` comes last, on line 9.
 */
[[nodiscard]] std::string calmFileWith(std::string_view key,
                                       std::string_view value);

/** @returns calmFile() without the line of @p key. */
[[nodiscard]] std::string calmFileWithout(std::string_view key);

/**
 * @returns The message of the ServerException reading @p text throws; an empty
 * text, after a failure, when it throws none.
 */
[[nodiscard]] std::string errorReading(std::string_view text);

}  // namespace rtype::server::testing
