#include "ConnectionAcceptorLogFixture.hpp"
#include <string>
#include <vector>
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server::testing {

std::vector<std::string> ConnectionAcceptorLogFixture::dropLines() const {
  return journal_.linesHolding(DROPPED_DATAGRAM_LINE_TEXT);
}

}  // namespace rtype::server::testing
