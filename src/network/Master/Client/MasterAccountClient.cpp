#include "MasterAccountClient.hpp"
#include <httplib.h>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include "ExchangeReceipt.hpp"
#include "ExchangeRequest.hpp"
#include "LookPurchase.hpp"
#include "MasterHttpConstants.hpp"
#include "MasterMessageConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterRoute.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"
#include "NetworkException.hpp"
#include "OwnedLooks.hpp"
#include "User.hpp"
#include "UserRole.hpp"
#include "Wallet.hpp"

// NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks)
namespace rtype::network {

namespace {

/** @returns The request of @p route, for @p path, carrying @p body in JSON
 * when it is not empty. */
httplib::Request requestOf(MasterRoute route, const std::string& path,
                           const std::string& body) {
  httplib::Request request;
  request.method = std::string{methodNameOf(route.method)};
  request.path = path;
  if (!body.empty()) {
    request.body = body;
    request.set_header(std::string{CONTENT_TYPE_HEADER},
                       std::string{JSON_CONTENT_TYPE});
  }
  return request;
}

/** @returns The path of BUY_OWN_LOOK_ROUTE for the look @p item:
 * `/api/users/me/looks/crown`. The names of the looks are lowercase letters,
 * written as they come. */
std::string ownLookPathOf(std::string_view item) {
  std::string path = apiPathOf(BUY_OWN_LOOK_ROUTE);
  const std::string placeholder = std::string{PATH_PARAMETER_PREFIX} +
                                  std::string{LOOK_ITEM_PATH_PARAMETER};
  path.replace(path.find(placeholder), placeholder.size(), item);
  return path;
}

/**
 * @returns The account the master answered as @p text: an object whose `id`
 * is a whole number of at least 1, whose `nickname` is a text and whose
 * `role` is one of USER_ROLE_TEXTS.
 * @throws InvalidMasterMessageException naming the first member that is not.
 */
User ownUserFromText(std::string_view text) {
  const nlohmann::json document = nlohmann::json::parse(text, nullptr, false);
  if (!document.is_object()) {
    throw InvalidMasterMessageException("");
  }
  const auto id = document.find(ID_MEMBER);
  if (id == document.end() || !id->is_number_integer() ||
      id->get<std::int64_t>() < 1) {
    throw InvalidMasterMessageException("/" + std::string{ID_MEMBER});
  }
  const auto nickname = document.find(NICKNAME_MEMBER);
  if (nickname == document.end() || !nickname->is_string()) {
    throw InvalidMasterMessageException("/" + std::string{NICKNAME_MEMBER});
  }
  const auto role = document.find(ROLE_MEMBER);
  const std::optional<UserRole> userRole =
      role != document.end() && role->is_string()
          ? userRoleOf(role->get<std::string>())
          : std::nullopt;
  if (!userRole.has_value()) {
    throw InvalidMasterMessageException("/" + std::string{ROLE_MEMBER});
  }
  return User{.id = id->get<std::int64_t>(),
              .nickname = nickname->get<std::string>(),
              .role = *userRole};
}

}  // namespace

MasterAccountClient::MasterAccountClient(const std::string& url)
    : transport_(url) {}

User MasterAccountClient::readOwnUser(const std::string& token) {
  return ownUserFromText(transport_.send(
      requestOf(READ_OWN_USER_ROUTE, apiPathOf(READ_OWN_USER_ROUTE), {}),
      token));
}

Wallet MasterAccountClient::readOwnWallet(const std::string& token) {
  return walletFromText(transport_.send(
      requestOf(READ_OWN_WALLET_ROUTE, apiPathOf(READ_OWN_WALLET_ROUTE), {}),
      token));
}

OwnedLooks MasterAccountClient::listOwnLooks(const std::string& token) {
  return ownedLooksFromText(transport_.send(
      requestOf(LIST_OWN_LOOKS_ROUTE, apiPathOf(LIST_OWN_LOOKS_ROUTE), {}),
      token));
}

LookPurchase MasterAccountClient::buyOwnLook(const std::string& token,
                                             std::string_view item) {
  return lookPurchaseFromText(transport_.send(
      requestOf(BUY_OWN_LOOK_ROUTE, ownLookPathOf(item), {}), token));
}

ExchangeReceipt MasterAccountClient::exchange(const std::string& token,
                                              const ExchangeRequest& request) {
  return exchangeReceiptFromText(transport_.send(
      requestOf(CREATE_OWN_EXCHANGE_ROUTE, apiPathOf(CREATE_OWN_EXCHANGE_ROUTE),
                exchangeRequestText(request)),
      token));
}

}  // namespace rtype::network
// NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks)
