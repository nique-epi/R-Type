#include "JournalLine.hpp"
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include "JournalLineParts.hpp"
#include "LoggingConstants.hpp"

namespace rtype::logging {

std::string jsonLineOf(const JournalLineParts& parts) {
  nlohmann::ordered_json line;
  line[std::string{TIME_KEY}] = parts.time;
  line[std::string{LEVEL_KEY}] = parts.level;
  line[std::string{LOGGER_KEY}] = parts.logger;
  line[std::string{MESSAGE_KEY}] = parts.message;
  return line.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace);
}

}  // namespace rtype::logging
