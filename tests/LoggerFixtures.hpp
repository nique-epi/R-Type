#pragma once

#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

/** @brief A value that counts how many times it was written to a stream, to
 * tell whether a filtered line was formatted. */
struct CountedValue {
  int* formattedCount{nullptr};
};

/** @brief Writes "counted" and adds one to the count of @p value. */
std::ostream& operator<<(std::ostream& stream, const CountedValue& value);

/** @returns The hour of now in UTC as the journal writes it: YYYY-MM-DDTHH. */
[[nodiscard]] std::string universalHourPrefix();

/** @returns @p text, one string per line. */
[[nodiscard]] std::vector<std::string> splitLines(const std::string& text);

/** @returns The pattern of a line of the journal of @p label, @p loggerName
 * and @p body, without colour. */
[[nodiscard]] std::string plainLinePattern(const std::string& label,
                                           const std::string& loggerName,
                                           const std::string& body);

/** @returns The pattern of the same line on standard error, with or without
 * colour around it. */
[[nodiscard]] std::string standardErrorLinePattern(
    const std::string& label, const std::string& loggerName,
    const std::string& body);

/** @returns How many times @p needle starts in @p text, overlaps included. */
[[nodiscard]] std::size_t countOccurrences(const std::string& text,
                                           const std::string& needle);
