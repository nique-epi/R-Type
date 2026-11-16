#pragma once

#include <chrono>
#include <string>
#include <string_view>
#include "Currency.hpp"
#include "ExchangeReceipt.hpp"
#include "ExchangeRequest.hpp"
#include "OwnedLooks.hpp"
#include "User.hpp"
#include "UserRole.hpp"
#include "Wallet.hpp"
#include "WalletExchange.hpp"

/** @brief The token of the session the tests of the account client send, in
 * the shape of a token of the master. */
constexpr std::string_view ACCOUNT_TOKEN =
    "rt_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
/** @brief How it reaches the master: `Bearer <token>`. */
inline const std::string BEARER_ACCOUNT_TOKEN =
    "Bearer " + std::string{ACCOUNT_TOKEN};

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

/** @brief The account of the token. */
inline const rtype::network::User ACE{
    .id = 7, .nickname = "ACE", .role = rtype::network::UserRole::Player};

/** @brief The balances of its wallet, and after a purchase or an exchange. */
constexpr rtype::network::Wallet ACE_WALLET{.kopecks = 1250, .chips = 40};
constexpr rtype::network::Wallet WALLET_AFTER{.kopecks = 750, .chips = 540};

/** @brief The looks it owns: the starter kit and the crown. */
inline const rtype::network::OwnedLooks ACE_LOOKS{
    .items = {"top_hat", "crown", "cool_shades", "mustache"}};

/** @brief 500 kopecks changed into chips, and the receipt of the cashier. */
constexpr rtype::network::ExchangeRequest BUY_500_CHIPS{
    .from = rtype::network::Currency::Kopecks, .amount = 500};
inline const rtype::network::ExchangeReceipt BUY_500_CHIPS_RECEIPT{
    .exchange = {.id = 3,
                 .from = rtype::network::Currency::Kopecks,
                 .amount = 500,
                 .madeAt = std::chrono::sys_seconds{std::chrono::seconds{
                     1'790'000'000}}},
    .wallet = WALLET_AFTER};

/** @brief An answer that is no account: an object without its members. */
constexpr std::string_view NO_ACCOUNT_TEXT = "{}";
