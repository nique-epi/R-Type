#include "GameCreditRoutesFixtures.hpp"
#include <cstdint>
#include <string>
#include "AccountsTestConstants.hpp"
#include "FinishedGame.hpp"
#include "LeaderboardRoutesFixtures.hpp"

rtype::network::FinishedGame gameOfAceWith(const std::string& ticket,
                                           std::uint32_t score) {
  return normalGameOfTwo({ticketedScoring(ACE_NICKNAME, ticket, score)});
}
