#include "TauntsFixtures.hpp"
#include <cstddef>
#include <string>
#include <string_view>
#include "FlightLogTestConstants.hpp"
#include "TauntConstants.hpp"

using rtype::client::testing::LONGEST_NICKNAME;
using rtype::client::testing::NICKNAME_LETTER;

std::string withLongestName(std::string line) {
  for (const std::string_view mark :
       {rtype::client::BEST_PILOT_MARK, rtype::client::LAST_PILOT_MARK,
        rtype::client::ZERO_PILOT_MARK}) {
    if (const std::size_t found = line.find(mark); found != std::string::npos) {
      line.replace(found, mark.size(),
                   std::string(LONGEST_NICKNAME, NICKNAME_LETTER));
    }
  }
  return line;
}
