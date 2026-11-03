#include "DatagramSorterLogFixture.hpp"
#include <string>
#include <vector>
#include "ServerLobbyTestConstants.hpp"

std::vector<std::string> DatagramSorterLogFixture::dropLines() const {
  return journal_.linesHolding(
      rtype::server::testing::DROPPED_DATAGRAM_LINE_TEXT);
}
