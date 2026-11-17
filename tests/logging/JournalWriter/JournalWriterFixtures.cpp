#include "JournalWriterFixtures.hpp"
#include <cstddef>
#include <string>
#include <vector>
#include "JournalWriter.hpp"
#include "JournalWriterTestConstants.hpp"
#include "LogLevel.hpp"
#include "ReceivedJournalLine.hpp"

void queueNumber(rtype::logging::JournalWriter& writer, std::size_t number) {
  writer.queue(rtype::logging::LogLevel::Info, WRITER_TEST_LOGGER_NAME,
               std::to_string(number));
}

std::vector<std::string> queuedTextsOf(
    const std::vector<ReceivedJournalLine>& lines) {
  std::vector<std::string> texts;
  texts.reserve(lines.size());
  for (const ReceivedJournalLine& line : lines) {
    texts.push_back(line.text);
  }
  return texts;
}

std::vector<std::string> numbersBelow(std::size_t count) {
  std::vector<std::string> numbers;
  numbers.reserve(count);
  for (std::size_t number = 0; number < count; ++number) {
    numbers.push_back(std::to_string(number));
  }
  return numbers;
}

std::size_t lostLinesOf(const std::vector<ReceivedJournalLine>& lines) {
  std::size_t lost = 0;
  for (const ReceivedJournalLine& line : lines) {
    lost += line.lostBefore;
  }
  return lost;
}
