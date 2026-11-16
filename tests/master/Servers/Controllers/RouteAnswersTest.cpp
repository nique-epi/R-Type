#include <gtest/gtest.h>
#include <functional>
#include <optional>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "DatedRequest.hpp"
#include "HttpHeader.hpp"
#include "MasterTestConstants.hpp"
#include "OneSessionAccounts.hpp"
#include "RouteAnswers.hpp"
#include "RouteAnswersFixtures.hpp"
#include "RouteAnswersTestConstants.hpp"

using rtype::master::answerAsAccountOf;
using rtype::master::DatedRequest;

/**
 * Given the session of ACE
 * When a route answers a request carrying its token
 * Then it answers for ACE, and read the session at the date of the request
 */
TEST(RouteAnswers, AnswersForTheAccountOfTheSession) {
  OneSessionAccounts accounts = accountsOfAce();
  const rtype::network::ApiRequest request =
      requestWithToken(KNOWN_SESSION_TOKEN);

  const rtype::network::ApiResponse answer = answerAsAccountOf(
      accounts, DatedRequest{.request = std::cref(request), .now = ANSWERED_AT},
      answerNaming);

  EXPECT_EQ(answer.status, ANSWER_OF_THE_ACCOUNT_STATUS);
  EXPECT_EQ(answer.body, SESSION_ACCOUNT_NICKNAME);
  EXPECT_EQ(accounts.lastAskedAt(), std::optional{ANSWERED_AT});
}

/**
 * Given the session of ACE
 * When a route answers a request carrying a token no session has
 * Then it answers 401 with `WWW-Authenticate: Bearer`, without answering
 */
TEST(RouteAnswers, RefusesATokenNoSessionHas) {
  OneSessionAccounts accounts = accountsOfAce();
  const rtype::network::ApiRequest request =
      requestWithToken(UNKNOWN_SESSION_TOKEN);

  const rtype::network::ApiResponse answer = answerAsAccountOf(
      accounts, DatedRequest{.request = std::cref(request), .now = ANSWERED_AT},
      answerNaming);

  EXPECT_EQ(answer.status, EXPECTED_UNAUTHORIZED);
  ASSERT_TRUE(answer.header.has_value());
  EXPECT_EQ(answer.header.value_or(rtype::network::HttpHeader{}).value,
            BEARER_CHALLENGE);
}

/**
 * Given the session of ACE
 * When a route answers a request carrying no token at all
 * Then it answers 401
 */
TEST(RouteAnswers, RefusesARequestWithoutAToken) {
  OneSessionAccounts accounts = accountsOfAce();
  const rtype::network::ApiRequest request = requestWithToken({});

  const rtype::network::ApiResponse answer = answerAsAccountOf(
      accounts, DatedRequest{.request = std::cref(request), .now = ANSWERED_AT},
      answerNaming);

  EXPECT_EQ(answer.status, EXPECTED_UNAUTHORIZED);
}
