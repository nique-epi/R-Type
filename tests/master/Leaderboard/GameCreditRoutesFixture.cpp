#include "GameCreditRoutesFixture.hpp"
#include <httplib.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "AccountLeaderboardRoutesTestConstants.hpp"
#include "AccountsRoutesFixtures.hpp"
#include "AccountsRoutesTestConstants.hpp"
#include "CreatedServer.hpp"
#include "FinishedGame.hpp"
#include "GameCreditRoutesTestConstants.hpp"
#include "GameTicketRoutesTestConstants.hpp"
#include "LeaderboardRoutesTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterTestConstants.hpp"
#include "RouteCall.hpp"
#include "RouteCallFixtures.hpp"
#include "ServersRoutesTestConstants.hpp"
#include "UserCredentials.hpp"
#include "WalletRoutesFixtures.hpp"
#include "WalletRoutesTestConstants.hpp"

namespace {

/** @returns The token of a session of the new account of @p credentials on
 * the master listening on @p port. */
std::string tokenOfNewAccount(
    std::uint16_t port, const rtype::network::UserCredentials& credentials) {
  static_cast<void>(
      sendRouteCall(port, credentialsCall(USERS_URL, credentials)));
  return memberIn(
      sendRouteCall(port, credentialsCall(SESSIONS_URL, credentials)),
      TOKEN_MEMBER_NAME);
}

}  // namespace

GameCreditRoutesFixture::GameCreditRoutesFixture()
    : paris_(master_.createServer(TEST_SERVER_NAME)),
      aceToken_(tokenOfNewAccount(master_.port(), aceCredentials())),
      bobToken_(tokenOfNewAccount(master_.port(),
                                  rtype::network::UserCredentials{
                                      .nickname = std::string{BOB_NICKNAME},
                                      .password = std::string{BOB_PASSWORD}})) {
  aceAccountId_ = std::to_string(
      numberIn(sendRouteCall(master_.port(), readOwnUserCall(aceToken_)),
               ACCOUNT_ID_MEMBER_NAME)
          .value_or(0));
}

const std::string& GameCreditRoutesFixture::aceAccountId() const {
  return aceAccountId_;
}

std::string GameCreditRoutesFixture::aceTicket() const {
  return memberIn(
      sendRouteCall(master_.port(),
                    ownCall(POST_METHOD, OWN_TICKETS_URL, aceToken_)),
      TICKET_MEMBER_NAME);
}

std::string GameCreditRoutesFixture::bobTicket() const {
  return memberIn(
      sendRouteCall(master_.port(),
                    ownCall(POST_METHOD, OWN_TICKETS_URL, bobToken_)),
      TICKET_MEMBER_NAME);
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

httplib::Result GameCreditRoutesFixture::readAccountLeaderboard(
    std::string_view query) const {
  const std::string path =
      std::string{ACCOUNT_LEADERBOARD_URL} + std::string{query};
  return sendRouteCall(master_.port(), RouteCall{.method = GET_METHOD_TEXT,
                                                 .path = path,
                                                 .key = {},
                                                 .body = {},
                                                 .overHttps = false});
}

std::optional<std::int64_t> GameCreditRoutesFixture::kopecksOfAce() const {
  return numberIn(
      sendRouteCall(master_.port(),
                    ownCall(GET_METHOD_TEXT, OWN_WALLET_URL, aceToken_)),
      KOPECKS_MEMBER_NAME);
}

std::optional<std::int64_t> GameCreditRoutesFixture::kopecksOfBob() const {
  return numberIn(
      sendRouteCall(master_.port(),
                    ownCall(GET_METHOD_TEXT, OWN_WALLET_URL, bobToken_)),
      KOPECKS_MEMBER_NAME);
}

std::optional<std::size_t> GameCreditRoutesFixture::looksOfAce() const {
  return arraySizeIn(
      sendRouteCall(master_.port(),
                    ownCall(GET_METHOD_TEXT, OWN_LOOKS_URL, aceToken_)),
      ITEMS_MEMBER_NAME);
}
