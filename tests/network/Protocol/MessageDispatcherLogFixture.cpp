#include "MessageDispatcherLogFixture.hpp"
#include <string>
#include <vector>
#include "DebugJournal.hpp"
#include "ProtocolTestConstants.hpp"

namespace rtype::network::testing {

std::vector<std::string> MessageDispatcherLogFixture::rejectionLines() const {
  return journal_.linesHolding(REJECTED_MESSAGE_LINE_TEXT);
}

const DebugJournal& MessageDispatcherLogFixture::journal() const {
  return journal_;
}

}  // namespace rtype::network::testing
