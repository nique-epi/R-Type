#pragma once

#include <span>
#include <string_view>
#include "EventField.hpp"
#include "LogLevel.hpp"

namespace rtype::logging {

/**
 * @brief Something that happened, written as one line: the name a tool filters
 * on ("player.joined"), the sentence a person reads, and the values of this
 * occurrence. Every view lives until Logger::event() returns.
 */
struct LogEvent {
  LogLevel level{LogLevel::Info};
  std::string_view name;
  std::string_view message;
  std::span<const EventField> fields;
};

}  // namespace rtype::logging
