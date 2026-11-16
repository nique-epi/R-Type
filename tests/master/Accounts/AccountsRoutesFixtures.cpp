#include "AccountsRoutesFixtures.hpp"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include "AccountsRoutesTestConstants.hpp"
#include "AccountsTestConstants.hpp"
#include "MasterMessageText.hpp"
#include "RouteCall.hpp"
#include "ServersRoutesTestConstants.hpp"
#include "UserCredentials.hpp"

rtype::network::UserCredentials aceCredentials() {
  return aceWith(ACE_PASSWORD);
}

rtype::network::UserCredentials aceWith(std::string_view password) {
  return rtype::network::UserCredentials{.nickname = std::string{ACE_NICKNAME},
                                         .password = std::string{password}};
}

RouteCall credentialsCall(std::string_view path,
                          const rtype::network::UserCredentials& credentials) {
  return RouteCall{.method = POST_METHOD,
                   .path = path,
                   .key = {},
                   .body = rtype::network::userCredentialsText(credentials),
                   .overHttps = true};
}

RouteCall readOwnUserCall(std::string_view token) {
  return RouteCall{.method = GET_METHOD_TEXT,
                   .path = OWN_ACCOUNT_URL,
                   .key = token,
                   .body = {},
                   .overHttps = true};
}

RouteCall deleteSessionCall(std::string_view token) {
  return RouteCall{.method = DELETE_METHOD_TEXT,
                   .path = OWN_SESSION_URL,
                   .key = token,
                   .body = {},
                   .overHttps = true};
}

std::string memberIn(const httplib::Result& answer, std::string_view member) {
  if (!answer) {
    return {};
  }
  const nlohmann::json document =
      nlohmann::json::parse(answer->body, nullptr, false);
  if (!document.is_object() || !document.contains(member) ||
      !document.at(member).is_string()) {
    return {};
  }
  return document.at(member).get<std::string>();
}
