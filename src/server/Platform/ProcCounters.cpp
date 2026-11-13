#include "ProcCounters.hpp"
#include <algorithm>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <system_error>
#include <vector>
#include "MachineCounters.hpp"
#include "MachineCountersConstants.hpp"
#include "ProcTexts.hpp"

namespace rtype::server {

namespace {

/** @returns What follows @p start on the line of @p text that starts with
 * it, up to the end of that line; nothing when no line does. */
std::optional<std::string_view> restOfLineStarting(std::string_view text,
                                                   std::string_view start) {
  std::size_t lineStart = 0;
  while (lineStart < text.size()) {
    const std::size_t lineEnd =
        std::min(text.find('\n', lineStart), text.size());
    const std::string_view line = text.substr(lineStart, lineEnd - lineStart);
    if (line.starts_with(start)) {
      return line.substr(start.size());
    }
    lineStart = lineEnd + 1;
  }
  return std::nullopt;
}

/** @returns The whole numbers @p fields starts with, separated by blanks, up
 * to the first word that is none. */
std::vector<std::uint64_t> leadingNumbersOf(std::string_view fields) {
  std::vector<std::uint64_t> numbers;
  std::size_t wordStart = fields.find_first_not_of(PROC_FIELD_BLANKS);
  while (wordStart != std::string_view::npos) {
    const std::size_t wordEnd = std::min(
        fields.find_first_of(PROC_FIELD_BLANKS, wordStart), fields.size());
    const std::string_view word = fields.substr(wordStart, wordEnd - wordStart);
    std::uint64_t number = 0;
    const std::from_chars_result read =
        std::from_chars(word.data(), word.data() + word.size(), number);
    if (read.ec != std::errc{} || read.ptr != word.data() + word.size()) {
      break;
    }
    numbers.push_back(number);
    wordStart = fields.find_first_not_of(PROC_FIELD_BLANKS, wordEnd);
  }
  return numbers;
}

/** @returns The bytes the line of @p memory that starts with @p start counts
 * in kibibytes; nothing when the line is missing or holds no number. */
std::optional<std::uint64_t> bytesOfLine(std::string_view memory,
                                         std::string_view start) {
  const std::optional<std::string_view> rest =
      restOfLineStarting(memory, start);
  if (!rest.has_value()) {
    return std::nullopt;
  }
  const std::vector<std::uint64_t> numbers = leadingNumbersOf(*rest);
  if (numbers.empty()) {
    return std::nullopt;
  }
  return numbers.front() * BYTES_PER_KIBIBYTE;
}

/** @returns The times of the `cpu` line of @p processors; nothing when it
 * holds fewer than FEWEST_PROCESSOR_FIELDS numbers. */
std::optional<std::vector<std::uint64_t>> processorTimesIn(
    std::string_view processors) {
  const std::optional<std::string_view> rest =
      restOfLineStarting(processors, ALL_PROCESSORS_LINE_START);
  if (!rest.has_value()) {
    return std::nullopt;
  }
  std::vector<std::uint64_t> times = leadingNumbersOf(*rest);
  if (times.size() < FEWEST_PROCESSOR_FIELDS) {
    return std::nullopt;
  }
  times.resize(PROCESSOR_TIME_FIELD_COUNT, 0);
  return times;
}

}  // namespace

std::optional<MachineCounters> countersIn(const ProcTexts& texts) {
  const std::optional<std::vector<std::uint64_t>> times =
      processorTimesIn(texts.processors);
  const std::optional<std::uint64_t> total =
      bytesOfLine(texts.memory, TOTAL_MEMORY_LINE_START);
  const std::optional<std::uint64_t> available =
      bytesOfLine(texts.memory, AVAILABLE_MEMORY_LINE_START);
  if (!times.has_value() || !total.has_value() || !available.has_value() ||
      *available > *total) {
    return std::nullopt;
  }
  std::uint64_t processorTime = 0;
  for (const std::uint64_t time : *times) {
    processorTime += time;
  }
  return MachineCounters{
      .processorTime = processorTime,
      .idleProcessorTime = times->at(IDLE_FIELD) + times->at(DISK_WAIT_FIELD),
      .usedMemoryBytes = *total - *available,
      .totalMemoryBytes = *total};
}

}  // namespace rtype::server
