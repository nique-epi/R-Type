#pragma once

#include <chrono>
#include <string_view>
#include "Currency.hpp"
#include "ExchangeRequest.hpp"
#include "OwnedLooks.hpp"
#include "User.hpp"
#include "UserRole.hpp"
#include "Wallet.hpp"

namespace rtype::client::testing {

/** @brief How long a test waits for the thread of the link to answer, and
 * how often it looks: far more than a call to the loopback takes. */
constexpr std::chrono::seconds LINK_READING_DEADLINE{10};
constexpr std::chrono::milliseconds LINK_POLL_STEP{10};

/** @brief The token the tests sign in with. */
constexpr std::string_view LINK_TOKEN =
    "rt_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";

/** @brief The routes of the account, as FakeAccountMaster names them. */
constexpr std::string_view OWN_USER_ROUTE = "GET /api/users/me";
constexpr std::string_view OWN_WALLET_ROUTE = "GET /api/users/me/wallet";
constexpr std::string_view OWN_LOOKS_ROUTE = "GET /api/users/me/looks";
constexpr std::string_view BUY_CROWN_ROUTE = "PUT /api/users/me/looks/crown";
constexpr std::string_view OWN_EXCHANGES_ROUTE =
    "POST /api/users/me/wallet/exchanges";

/** @brief The look the tests buy. */
constexpr std::string_view LINK_CROWN = "crown";

/** @brief The statuses the master answers with. */
constexpr int LINK_OK_STATUS = 200;
constexpr int LINK_CREATED_STATUS = 201;
constexpr int LINK_UNAUTHORIZED_STATUS = 401;
constexpr int LINK_CONFLICT_STATUS = 409;
constexpr int LINK_SERVER_ERROR_STATUS = 500;

/** @brief The account of the token, its balances, before and after a
 * purchase or an exchange, and its looks. */
inline const network::User LINK_ACE{
    .id = 7, .nickname = "ACE", .role = network::UserRole::Player};
constexpr network::Wallet LINK_WALLET{.kopecks = 1250, .chips = 40};
constexpr network::Wallet LINK_WALLET_AFTER{.kopecks = 750, .chips = 540};
inline const network::OwnedLooks LINK_LOOKS{
    .items = {"top_hat", "cool_shades", "mustache"}};

/** @brief When the cashier made an exchange. */
constexpr std::chrono::sys_seconds LINK_MADE_AT{
    std::chrono::seconds{1'790'000'000}};

/** @brief 500 kopecks changed into chips. */
constexpr network::ExchangeRequest LINK_BUY_500_CHIPS{
    .from = network::Currency::Kopecks, .amount = 500};

}  // namespace rtype::client::testing
