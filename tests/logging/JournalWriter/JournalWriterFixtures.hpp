#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "JournalWriter.hpp"
#include "ReceivedJournalLine.hpp"

/** @brief Queues on @p writer the line that reads @p number. */
void queueNumber(rtype::logging::JournalWriter& writer, std::size_t number);

/** @returns The texts of @p lines, in order. */
[[nodiscard]] std::vector<std::string> queuedTextsOf(
    const std::vector<ReceivedJournalLine>& lines);

/** @returns "0" to "count - 1", in order. */
[[nodiscard]] std::vector<std::string> numbersBelow(std::size_t count);

/** @returns The sum of the lines lost before each of @p lines. */
[[nodiscard]] std::size_t lostLinesOf(
    const std::vector<ReceivedJournalLine>& lines);
