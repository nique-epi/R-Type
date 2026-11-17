#include "MasterAccountClient.hpp"
#include <string>
#include <string_view>
#include "ExchangeReceipt.hpp"
#include "ExchangeRequest.hpp"
#include "LookPurchase.hpp"
#include "MasterMessageText.hpp"
#include "MasterRequest.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"
#include "OwnedLooks.hpp"
#include "User.hpp"
#include "Wallet.hpp"

// NOLINTBEGIN(clang-analyzer-cplusplus.NewDeleteLeaks)
namespace rtype::network {

MasterAccountClient::MasterAccountClient(const std::string& url)
    : transport_(url) {}

User MasterAccountClient::readOwnUser(const std::string& token) {
  return userFromText(transport_.send(
      masterRequestOf(READ_OWN_USER_ROUTE, apiPathOf(READ_OWN_USER_ROUTE)),
      token));
}

Wallet MasterAccountClient::readOwnWallet(const std::string& token) {
  return walletFromText(transport_.send(
      masterRequestOf(READ_OWN_WALLET_ROUTE, apiPathOf(READ_OWN_WALLET_ROUTE)),
      token));
}

OwnedLooks MasterAccountClient::listOwnLooks(const std::string& token) {
  return ownedLooksFromText(transport_.send(
      masterRequestOf(LIST_OWN_LOOKS_ROUTE, apiPathOf(LIST_OWN_LOOKS_ROUTE)),
      token));
}

LookPurchase MasterAccountClient::buyOwnLook(const std::string& token,
                                             std::string_view item) {
  return lookPurchaseFromText(transport_.send(
      masterRequestOf(BUY_OWN_LOOK_ROUTE, lookPathOf(BUY_OWN_LOOK_ROUTE, item)),
      token));
}

ExchangeReceipt MasterAccountClient::exchange(const std::string& token,
                                              const ExchangeRequest& request) {
  return exchangeReceiptFromText(
      transport_.send(jsonRequestOf(CREATE_OWN_EXCHANGE_ROUTE,
                                    apiPathOf(CREATE_OWN_EXCHANGE_ROUTE),
                                    exchangeRequestText(request)),
                      token));
}

}  // namespace rtype::network
// NOLINTEND(clang-analyzer-cplusplus.NewDeleteLeaks)
