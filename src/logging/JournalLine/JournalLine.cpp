#include "JournalLine.hpp"
#include <algorithm>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include <variant>
#include "EventField.hpp"
#include "JournalLineParts.hpp"
#include "LogEvent.hpp"
#include "LoggingConstants.hpp"

namespace rtype::logging {

namespace {

/** @returns @p json as one line, a byte that is not UTF-8 replaced. */
std::string compactTextOf(const nlohmann::ordered_json& json) {
  return json.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace);
}

/** @returns Whether every line already has a member named @p name. */
bool isReservedName(std::string_view name) {
  return std::ranges::find(RESERVED_KEYS, name) != RESERVED_KEYS.end();
}

}  // namespace

std::string jsonLineOf(const JournalLineParts& parts) {
  nlohmann::ordered_json line;
  line[std::string{TIME_KEY}] = parts.time;
  line[std::string{LEVEL_KEY}] = parts.level;
  line[std::string{LOGGER_KEY}] = parts.logger;
  line[std::string{MESSAGE_KEY}] = parts.message;
  std::string text = compactTextOf(line);
  if (parts.fields.size() > 2) {
    text.back() = ',';
    text.append(parts.fields.substr(1));
  }
  return text;
}

std::string eventFieldsText(const LogEvent& event) {
  nlohmann::ordered_json fields;
  fields[std::string{EVENT_KEY}] = event.name;
  for (const EventField& field : event.fields) {
    if (!isReservedName(field.name)) {
      std::visit(
          [&fields, &field](const auto& value) {
            fields[std::string{field.name}] = value;
          },
          field.value);
    }
  }
  return compactTextOf(fields);
}

}  // namespace rtype::logging
