#include "GameCreditRoutesFixture.hpp"
#include <httplib.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "AccountsRoutesFixtures.hpp"
#include "AccountsRoutesTestConstants.hpp"
#include "CreatedServer.hpp"
#include "FinishedGame.hpp"
#include "GameCreditRoutesTestConstants.hpp"
#include "LeaderboardRoutesTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterTestConstants.hpp"
#include "RouteCall.hpp"
#include "RouteCallFixtures.hpp"
#include "ServersRoutesTestConstants.hpp"
#include "WalletRoutesFixtures.hpp"
#include "WalletRoutesTestConstants.hpp"

GameCreditRoutesFixture::GameCreditRoutesFixture()
    : paris_(master_.createServer(TEST_SERVER_NAME)) {
  static_cast<void>(sendRouteCall(
      master_.port(), credentialsCall(USERS_URL, aceCredentials())));
  token_ =
      memberIn(sendRouteCall(master_.port(),
                             credentialsCall(SESSIONS_URL, aceCredentials())),
               TOKEN_MEMBER_NAME);
  aceAccountId_ = std::to_string(
      numberIn(sendRouteCall(master_.port(), readOwnUserCall(token_)),
               ACCOUNT_ID_MEMBER_NAME)
          .value_or(0));
}

const std::string& GameCreditRoutesFixture::aceAccountId() const {
  return aceAccountId_;
}

httplib::Result GameCreditRoutesFixture::sendResult(
    std::string_view gameId, const rtype::network::FinishedGame& game) const {
  const std::string path =
      std::string{OWN_RESULT_URL_PREFIX} + std::string{gameId};
  return sendRouteCall(master_.port(),
                       RouteCall{.method = PUT_METHOD,
                                 .path = path,
                                 .key = paris_.key,
                                 .body = rtype::network::finishedGameText(game),
                                 .overHttps = false});
}

std::optional<std::int64_t> GameCreditRoutesFixture::kopecksOfAce() const {
  return numberIn(
      sendRouteCall(master_.port(),
                    ownCall(GET_METHOD_TEXT, OWN_WALLET_URL, token_)),
      KOPECKS_MEMBER_NAME);
}

std::optional<std::size_t> GameCreditRoutesFixture::looksOfAce() const {
  return arraySizeIn(
      sendRouteCall(master_.port(),
                    ownCall(GET_METHOD_TEXT, OWN_LOOKS_URL, token_)),
      ITEMS_MEMBER_NAME);
}
