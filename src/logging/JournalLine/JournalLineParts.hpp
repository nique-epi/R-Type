#pragma once

#include <string_view>

namespace rtype::logging {

/**
 * @brief What a line of the journal says, apart from how it is written: the
 * time of its call as text, the name of its level ("info"), the name of its
 * logger, its message and, for an event, the JSON object eventFieldsText()
 * made (empty for a plain line). The views must outlive the call that reads
 * them.
 */
struct JournalLineParts {
  std::string_view time;
  std::string_view level;
  std::string_view logger;
  std::string_view message;
  std::string_view fields;
};

}  // namespace rtype::logging
