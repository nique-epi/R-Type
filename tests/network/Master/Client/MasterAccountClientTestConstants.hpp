#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>
#include "Currency.hpp"
#include "ExchangeRequest.hpp"
#include "Wallet.hpp"

/** @brief The token of the session the tests of the account client send, in
 * the shape of a token of the master. */
constexpr std::string_view ACCOUNT_TOKEN =
    "rt_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
/** @brief How it reaches the master: `Bearer <token>`. */
constexpr std::string_view BEARER_ACCOUNT_TOKEN =
    "Bearer "
    "rt_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";

/** @brief The routes of the account, as FakeAccountMaster names them. */
constexpr std::string_view READ_OWN_USER = "GET /api/users/me";
constexpr std::string_view READ_OWN_WALLET = "GET /api/users/me/wallet";
constexpr std::string_view LIST_OWN_LOOKS = "GET /api/users/me/looks";
constexpr std::string_view BUY_CROWN = "PUT /api/users/me/looks/crown";
constexpr std::string_view CREATE_OWN_EXCHANGE =
    "POST /api/users/me/wallet/exchanges";

/** @brief The look the tests buy, and the path it is bought at. */
constexpr std::string_view CROWN = "crown";
constexpr std::string_view CROWN_PATH = "/api/users/me/looks/crown";

/** @brief The statuses the master answers with. */
constexpr int OK_STATUS = 200;
constexpr int CREATED_STATUS = 201;
constexpr int UNAUTHORIZED_STATUS = 401;
constexpr int CONFLICT_STATUS = 409;

/** @brief The account of the token (aceUser(),
 * MasterAccountClientFixtures.hpp). */
constexpr std::int64_t ACE_USER_ID = 7;
constexpr std::string_view ACE_USER_NICKNAME = "ACE";

/** @brief The balances of its wallet, and after a purchase or an exchange. */
constexpr rtype::network::Wallet ACE_WALLET{.kopecks = 1250, .chips = 40};
constexpr rtype::network::Wallet WALLET_AFTER{.kopecks = 750, .chips = 540};

/** @brief 500 kopecks changed into chips; the cashier numbers the exchange
 * BUY_500_CHIPS_ID and dates it BUY_500_CHIPS_MADE_AT
 * (buyFiveHundredChipsReceipt()). */
constexpr rtype::network::ExchangeRequest BUY_500_CHIPS{
    .from = rtype::network::Currency::Kopecks, .amount = 500};
constexpr std::int64_t BUY_500_CHIPS_ID = 3;
constexpr std::chrono::sys_seconds BUY_500_CHIPS_MADE_AT{
    std::chrono::seconds{1'790'000'000}};

/** @brief An answer that is no account: an object without its members. */
constexpr std::string_view NO_ACCOUNT_TEXT = "{}";
