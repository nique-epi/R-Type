#include "JournalLineFixtures.hpp"
#include <string_view>
#include "JournalLineParts.hpp"
#include "JournalLineTestConstants.hpp"

rtype::logging::JournalLineParts partsWithMessage(std::string_view message) {
  return rtype::logging::JournalLineParts{.time = LINE_TIME,
                                          .level = LINE_LEVEL,
                                          .logger = LINE_LOGGER,
                                          .message = message,
                                          .fields = {}};
}
