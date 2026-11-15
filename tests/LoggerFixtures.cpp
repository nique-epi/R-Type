#include "LoggerFixtures.hpp"
#include <array>
#include <chrono>
#include <cstddef>
#include <ctime>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include "LoggerTestConstants.hpp"

std::string universalHourPrefix() {
  const std::time_t now =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::tm universalTime{};
#if defined(_WIN32)
  (void)gmtime_s(&universalTime, &now);
#else
  (void)gmtime_r(&now, &universalTime);
#endif
  std::array<char, HOUR_PREFIX_BUFFER_SIZE> buffer{};
  (void)std::strftime(buffer.data(), buffer.size(), "%Y-%m-%dT%H",
                      &universalTime);
  return buffer.data();
}

std::ostream& operator<<(std::ostream& stream, const CountedValue& value) {
  ++*value.formattedCount;
  return stream << "counted";
}

std::vector<std::string> splitLines(const std::string& text) {
  std::vector<std::string> lines;
  std::istringstream stream(text);
  std::string line;
  while (std::getline(stream, line)) {
    lines.push_back(line);
  }
  return lines;
}

std::string plainLinePattern(const std::string& label,
                             const std::string& loggerName,
                             const std::string& body) {
  return R"(\[\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.\d{3}Z\] \[)" + label +
         R"(\] \[)" + loggerName + R"(\] - )" + body;
}

std::string standardErrorLinePattern(const std::string& label,
                                     const std::string& loggerName,
                                     const std::string& body) {
  return R"((\x1b\[\d+m)?)" + plainLinePattern(label, loggerName, body) +
         R"((\x1b\[0m)?)";
}

std::size_t countOccurrences(const std::string& text,
                             const std::string& needle) {
  std::size_t count = 0;
  std::size_t position = 0;
  while ((position = text.find(needle, position)) != std::string::npos) {
    ++count;
    position += 1;
  }
  return count;
}
