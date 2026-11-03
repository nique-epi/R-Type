#include "MessageDispatcherFixture.hpp"
#include <string>
#include <string_view>
#include <vector>
#include "DebugJournal.hpp"
#include "ProtocolTestConstants.hpp"

namespace rtype::network::testing {

std::vector<std::string> MessageDispatcherFixture::rejectionLines() const {
  return journal_.linesHolding(REJECTED_MESSAGE_LINE_TEXT);
}

std::vector<std::string> MessageDispatcherFixture::linesHolding(
    std::string_view text) const {
  return journal_.linesHolding(text);
}

}  // namespace rtype::network::testing
