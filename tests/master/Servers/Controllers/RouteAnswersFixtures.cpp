#include "RouteAnswersFixtures.hpp"
#include <optional>
#include <string>
#include <string_view>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "OneSessionAccounts.hpp"
#include "RouteAnswersTestConstants.hpp"
#include "User.hpp"

OneSessionAccounts accountsOfAce() {
  return OneSessionAccounts{
      std::string{KNOWN_SESSION_TOKEN},
      rtype::network::User{.id = SESSION_ACCOUNT_ID,
                           .nickname = std::string{SESSION_ACCOUNT_NICKNAME}}};
}

rtype::network::ApiRequest requestWithToken(std::string_view token) {
  rtype::network::ApiRequest request;
  if (!token.empty()) {
    request.bearerToken = std::string{token};
  }
  return request;
}

rtype::network::ApiResponse answerNaming(const rtype::network::User& user) {
  return rtype::network::ApiResponse{.status = ANSWER_OF_THE_ACCOUNT_STATUS,
                                     .body = user.nickname};
}
