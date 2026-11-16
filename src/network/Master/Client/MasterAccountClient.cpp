#include "MasterAccountClient.hpp"
#include <httplib.h>
#include <string>
#include <string_view>
#include "ExchangeReceipt.hpp"
#include "ExchangeRequest.hpp"
#include "LookPurchase.hpp"
#include "MasterHttpConstants.hpp"
#include "MasterMessageText.hpp"
#include "MasterRoute.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"
#include "OwnedLooks.hpp"
#include "User.hpp"
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

}  // namespace

MasterAccountClient::MasterAccountClient(const std::string& url)
    : transport_(url) {}

User MasterAccountClient::readOwnUser(const std::string& token) {
  return userFromText(transport_.send(
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
