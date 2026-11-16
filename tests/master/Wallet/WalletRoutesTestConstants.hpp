#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

/** @brief The paths of the routes of the wallet. */
constexpr std::string_view OWN_WALLET_URL = "/api/users/me/wallet";
constexpr std::string_view OWN_MOVEMENTS_URL = "/api/users/me/wallet/movements";
constexpr std::string_view OWN_EXCHANGES_URL = "/api/users/me/wallet/exchanges";
constexpr std::string_view OWN_LOOKS_URL = "/api/users/me/looks";
constexpr std::string_view PARTY_HAT_URL = "/api/users/me/looks/party_hat";
constexpr std::string_view GOLDEN_BYDO_URL = "/api/users/me/looks/golden_bydo";
constexpr std::string_view UNKNOWN_LOOK_URL =
    "/api/users/me/looks/no_such_look";
constexpr std::string_view UNKNOWN_EXCHANGE_URL =
    "/api/users/me/wallet/exchanges/999";
constexpr std::string_view EXCHANGE_LOCATION_START =
    "/api/users/me/wallet/exchanges/";
constexpr std::string_view ONE_LINE_QUERY = "?limit=1";
constexpr std::string_view NO_LINE_QUERY = "?limit=0";

/** @brief The line that gives the wallets of the tests 1000 kopecks when they
 * open, the chips keeping their default of 500, and the line that turns the
 * wallets off. */
constexpr std::string_view STARTING_KOPECKS_LINE =
    "set wallet-starting-kopecks 1000";
constexpr std::int64_t STARTING_KOPECKS_SET = 1'000;
constexpr std::int64_t DEFAULT_STARTING_CHIPS_READ = 500;
constexpr std::string_view WALLET_MODULE_OFF_LINE = "module wallet off";

/** @brief Bodies of the cashier: 200 kopecks into chips, more kopecks than the
 * wallet holds, none, and a currency the cashier does not know. */
constexpr std::string_view TWO_HUNDRED_KOPECKS_BODY =
    R"({"from":"kopecks","amount":200})";
constexpr std::int64_t TWO_HUNDRED_EXCHANGED = 200;
constexpr std::string_view TOO_MANY_KOPECKS_BODY =
    R"({"from":"kopecks","amount":1001})";
constexpr std::string_view NO_AMOUNT_BODY = R"({"from":"chips","amount":0})";
constexpr std::string_view GOLD_BODY = R"({"from":"gold","amount":1})";

/** @brief The members the answers of the wallet hold. */
constexpr std::string_view KOPECKS_MEMBER_NAME = "kopecks";
constexpr std::string_view CHIPS_MEMBER_NAME = "chips";
constexpr std::string_view AMOUNT_MEMBER_NAME = "amount";
constexpr std::string_view ITEMS_MEMBER_NAME = "items";
constexpr std::string_view MOVEMENTS_MEMBER_NAME = "movements";

/** @brief What the party hat leaves of 1000 kopecks, and the looks of the
 * starter kit. */
constexpr std::int64_t KOPECKS_AFTER_THE_PARTY_HAT = 850;
constexpr std::size_t STARTER_LOOK_COUNT = 3;

/** @brief The lines a page of `?limit=1` holds. */
constexpr std::size_t LINES_OF_ONE_PAGE = 1;
