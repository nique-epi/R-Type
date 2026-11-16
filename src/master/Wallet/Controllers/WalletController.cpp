#include "WalletController.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include "ApiRequest.hpp"
#include "ApiResponse.hpp"
#include "ExchangeReceipt.hpp"
#include "ExchangeRequest.hpp"
#include "HttpConstants.hpp"
#include "HttpHeader.hpp"
#include "ISessionAccounts.hpp"
#include "LookItems.hpp"
#include "LookPurchase.hpp"
#include "LookPurchaseOutcome.hpp"
#include "LookPurchaseResult.hpp"
#include "MasterMessageText.hpp"
#include "MasterRouteConstants.hpp"
#include "MasterRoutePath.hpp"
#include "PageRequest.hpp"
#include "QueryBounds.hpp"
#include "RouteAnswers.hpp"
#include "ShipLookConstants.hpp"
#include "User.hpp"
#include "WalletControllerConstants.hpp"
#include "WalletExchange.hpp"
#include "WalletExchangeRefusal.hpp"
#include "WalletService.hpp"

namespace rtype::master {

namespace {

network::ApiResponse exchangeRefusalOf(network::WalletExchangeRefusal reason) {
  switch (reason) {
    case network::WalletExchangeRefusal::InvalidAmount:
      return refusal(UNPROCESSABLE_CONTENT_STATUS, INVALID_AMOUNT_DETAIL);
    case network::WalletExchangeRefusal::NotEnough:
      return refusal(CONFLICT_STATUS, NOT_ENOUGH_DETAIL);
    case network::WalletExchangeRefusal::PastTheMaximum:
      return refusal(CONFLICT_STATUS, PAST_THE_MAXIMUM_DETAIL);
  }
  return refusal(CONFLICT_STATUS, NOT_ENOUGH_DETAIL);
}

network::ApiResponse createdExchangeResponse(
    const network::ExchangeReceipt& receipt) {
  return network::ApiResponse{
      .status = CREATED_STATUS,
      .body = network::exchangeReceiptText(receipt),
      .detail = std::nullopt,
      .header = network::HttpHeader{
          .name = std::string{LOCATION_HEADER},
          .value = network::memberPathOf(network::CREATE_OWN_EXCHANGE_ROUTE,
                                         receipt.exchange.id)}};
}

network::ApiResponse exchangeResponseOf(
    const std::variant<network::ExchangeReceipt,
                       network::WalletExchangeRefusal>& outcome) {
  if (const auto* receipt = std::get_if<network::ExchangeReceipt>(&outcome)) {
    return createdExchangeResponse(*receipt);
  }
  return exchangeRefusalOf(std::get<network::WalletExchangeRefusal>(outcome));
}

/** @returns The number of the exchange the path of @p request names, nothing
 * when it is no number of a line of the journal. */
std::optional<std::int64_t> exchangeIdOf(const network::ApiRequest& request) {
  return pathNumberOf(request, network::EXCHANGE_ID_PATH_PARAMETER,
                      QueryBounds{.lowest = FIRST_EXCHANGE_ID,
                                  .highest = LAST_EXCHANGE_ID,
                                  .absent = FIRST_EXCHANGE_ID});
}

/** @returns The look the path of @p request names, nothing when it names
 * none. */
std::optional<std::size_t> lookItemOf(const network::ApiRequest& request) {
  const std::optional<std::string> text =
      pathTextOf(request, network::LOOK_ITEM_PATH_PARAMETER);
  if (!text.has_value()) {
    return std::nullopt;
  }
  return network::lookItemOfName(*text);
}

network::ApiResponse purchaseResponseOf(std::size_t item,
                                        const LookPurchaseResult& result) {
  const network::LookPurchase purchase{
      .item = std::string{identifiers::SHIP_LOOK_ITEM_NAMES.at(item)},
      .wallet = result.wallet};
  switch (result.outcome) {
    case LookPurchaseOutcome::Bought:
      return network::ApiResponse{.status = CREATED_STATUS,
                                  .body = network::lookPurchaseText(purchase)};
    case LookPurchaseOutcome::AlreadyOwned:
      return okResponse(network::lookPurchaseText(purchase));
    case LookPurchaseOutcome::NotForSale:
      return refusal(UNPROCESSABLE_CONTENT_STATUS, NOT_FOR_SALE_DETAIL);
    case LookPurchaseOutcome::NotEnoughKopecks:
      return refusal(CONFLICT_STATUS, NOT_ENOUGH_KOPECKS_DETAIL);
  }
  return refusal(CONFLICT_STATUS, NOT_ENOUGH_KOPECKS_DETAIL);
}

}  // namespace

WalletController::WalletController(ISessionAccounts& accounts,
                                   WalletService& service)
    : accounts_(accounts), service_(service) {}

network::ApiResponse WalletController::readOwnWallet(
    const network::ApiRequest& request) {
  return answerAsAccountOf(
      accounts_, request,
      [this](const network::User& user, std::chrono::sys_seconds now) {
        return okResponse(network::walletText(service_.walletOf(user.id, now)));
      });
}

network::ApiResponse WalletController::listOwnMovements(
    const network::ApiRequest& request) {
  return answerAsAccountOf(accounts_, request,
                           [this, &request](const network::User& user,
                                            std::chrono::sys_seconds now) {
                             const std::optional<PageRequest> page =
                                 pageOf(request);
                             if (!page.has_value()) {
                               return invalidPageRefusal();
                             }
                             return okResponse(network::movementListingText(
                                 service_.movementsOf(user.id, *page, now)));
                           });
}

network::ApiResponse WalletController::createOwnExchange(
    const network::ApiRequest& request) {
  return answerAsAccountOf(
      accounts_, request,
      [this, &request](const network::User& user,
                       std::chrono::sys_seconds now) {
        const network::ExchangeRequest exchange =
            network::exchangeRequestFromText(request.body);
        return exchangeResponseOf(service_.exchange(user.id, exchange, now));
      });
}

network::ApiResponse WalletController::readOwnExchange(
    const network::ApiRequest& request) {
  return answerAsAccountOf(
      accounts_, request,
      [this, &request](const network::User& user,
                       std::chrono::sys_seconds /*now*/) {
        const std::optional<std::int64_t> exchangeId = exchangeIdOf(request);
        const std::optional<network::WalletExchange> exchange =
            exchangeId.has_value() ? service_.exchangeOf(user.id, *exchangeId)
                                   : std::nullopt;
        if (!exchange.has_value()) {
          return refusal(NOT_FOUND_STATUS, UNKNOWN_EXCHANGE_DETAIL);
        }
        return okResponse(network::walletExchangeText(*exchange));
      });
}

network::ApiResponse WalletController::listOwnLooks(
    const network::ApiRequest& request) {
  return answerAsAccountOf(
      accounts_, request,
      [this](const network::User& user, std::chrono::sys_seconds now) {
        return okResponse(
            network::ownedLooksText(service_.ownedLooksOf(user.id, now)));
      });
}

network::ApiResponse WalletController::buyOwnLook(
    const network::ApiRequest& request) {
  return answerAsAccountOf(
      accounts_, request,
      [this, &request](const network::User& user,
                       std::chrono::sys_seconds now) {
        const std::optional<std::size_t> item = lookItemOf(request);
        if (!item.has_value()) {
          return refusal(NOT_FOUND_STATUS, UNKNOWN_LOOK_DETAIL);
        }
        return purchaseResponseOf(*item, service_.buyLook(user.id, *item, now));
      });
}

}  // namespace rtype::master
