#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "Wallet.hpp"

/** @brief How the tests add an account without hashing a password: a
 * player whose hash matches no password. */
constexpr std::string_view INSERT_TEST_USER_STATEMENT = R"sql(
INSERT INTO users (nickname, password_hash, role, created_by_address, created_at)
VALUES (:nickname, zeroblob(128), 'player', '127.0.0.1', 0)
RETURNING id
)sql";
constexpr std::string_view TEST_USER_NICKNAME_PARAMETER = ":nickname";
constexpr std::string_view WALLET_TEST_NICKNAME = "ACE";

/** @brief How the tests sum the journal of a currency of a wallet. */
constexpr std::string_view JOURNAL_SUM_QUERY = R"sql(
SELECT COALESCE(SUM(amount), 0) FROM wallet_movements
WHERE user_id = :user_id AND currency = :currency
)sql";
constexpr std::string_view JOURNAL_USER_PARAMETER = ":user_id";
constexpr std::string_view JOURNAL_CURRENCY_PARAMETER = ":currency";

/** @brief The column the statements of the tests read their one value in. */
constexpr int FIRST_RESULT_COLUMN = 0;

/** @brief The balances a wallet of the tests opens with. */
constexpr rtype::network::Wallet TEST_STARTING_WALLET{.kopecks = 1'000,
                                                      .chips = 500};

/** @brief The date the tests work at. */
constexpr std::chrono::sys_seconds WALLET_TEST_DATE{
    std::chrono::seconds{1'800'000'000}};

/** @brief An id no account has. */
constexpr std::int64_t UNKNOWN_USER_ID = 999;

/** @brief Amounts the tests change at the cashier. */
constexpr std::uint32_t TWO_HUNDRED = 200;
constexpr std::uint32_t MORE_THAN_THE_STARTING_KOPECKS = 1'001;
constexpr std::uint32_t PAST_THE_LARGEST_BALANCE = 1'000'000;
constexpr std::uint32_t NO_AMOUNT = 0;

/** @brief The party hat, at 150 kopecks; the top hat, a starter look the
 * shop does not sell; the golden Bydo, at 10000 kopecks. */
constexpr std::size_t PARTY_HAT_ITEM = 1;
constexpr std::uint32_t PARTY_HAT_PRICE_PAID = 150;
constexpr std::size_t TOP_HAT_ITEM = 0;
constexpr std::size_t GOLDEN_BYDO_ITEM = 29;

/** @brief The starter looks a wallet opens with, in the order of the
 * catalogue. */
constexpr std::string_view STARTER_TOP_HAT_NAME = "top_hat";
constexpr std::string_view STARTER_COOL_SHADES_NAME = "cool_shades";
constexpr std::string_view STARTER_MUSTACHE_NAME = "mustache";

/** @brief Chips the casino stakes in the tests, and the reference of a
 * stake. */
constexpr std::int64_t A_STAKE_OF_A_HUNDRED = -100;
constexpr std::int64_t A_STAKE_PAST_THE_CHIPS = -501;
constexpr std::string_view STAKE_REFERENCE = "round:1:43:1";

/** @brief A game the tests credit: its kopecks, a look won and a name of no
 * look, and its reference. */
constexpr std::uint32_t GAME_KOPECKS = 1'245;
constexpr std::string_view WON_LOOK_NAME = "crown";
constexpr std::string_view NO_LOOK_NAME = "no_such_look";
constexpr std::string_view GAME_REFERENCE = "game:7:0123456789abcdef:ACE";

/** @brief What the transaction of a test throws to roll back. */
constexpr std::string_view ROLLED_BACK_TEXT = "rolled back";

/** @brief How the tests forget a look an account owns. */
constexpr std::string_view FORGET_LOOK_STATEMENT =
    "DELETE FROM owned_looks WHERE user_id = :user_id AND item = :item";
constexpr std::string_view FORGET_LOOK_ITEM_PARAMETER = ":item";

/** @brief A page of one line, the first. */
constexpr std::int64_t ONE_LINE = 1;
constexpr std::int64_t FROM_THE_FIRST = 0;
