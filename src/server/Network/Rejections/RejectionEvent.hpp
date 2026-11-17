#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "EventField.hpp"

namespace rtype::server {

/**
 * @brief One event of a rejection report: its name, the sentence the journal
 * shows, and its values. The fields point into the report and into constants,
 * so the report must outlive it.
 */
struct RejectionEvent {
  std::string_view name;
  std::string message;
  std::vector<logging::EventField> fields;
};

}  // namespace rtype::server
