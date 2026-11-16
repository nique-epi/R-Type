#pragma once

#include <array>
#include <string_view>

namespace rtype::master {

/** @brief The statements of the table `wallets`. An opening inserts nothing
 * for an id no account has, so a credit for an unknown account opens
 * nothing. */
constexpr std::string_view WALLET_BALANCES_QUERY =
    "SELECT kopecks, chips FROM wallets WHERE user_id = :user_id";
constexpr std::string_view INSERT_OPENED_WALLET_STATEMENT = R"sql(
INSERT INTO wallets (user_id, kopecks, chips, opened_at)
SELECT id, :kopecks, :chips, :opened_at FROM users WHERE id = :user_id
ON CONFLICT (user_id) DO NOTHING
)sql";
constexpr std::string_view REPLACE_BALANCES_STATEMENT =
    "UPDATE wallets SET kopecks = :kopecks, chips = :chips "
    "WHERE user_id = :user_id";

/** @brief The statements of the table `wallet_movements`. */
constexpr std::string_view NEXT_MOVEMENT_ID_QUERY =
    "SELECT COALESCE(MAX(id), 0) + 1 FROM wallet_movements";
constexpr std::string_view INSERT_MOVEMENT_STATEMENT = R"sql(
INSERT INTO wallet_movements
  (id, user_id, currency, amount, reason, reference, made_at)
VALUES (:id, :user_id, :currency, :amount, :reason, :reference, :made_at)
RETURNING id
)sql";
constexpr std::string_view MOVEMENT_PAGE_QUERY = R"sql(
SELECT id, currency, amount, reason, made_at FROM wallet_movements
WHERE user_id = :user_id
ORDER BY id DESC LIMIT :limit OFFSET :offset
)sql";
constexpr std::string_view EXCHANGE_DEBIT_QUERY = R"sql(
SELECT id, currency, amount, reason, made_at FROM wallet_movements
WHERE user_id = :user_id AND id = :id AND reason = :reason AND amount < 0
)sql";
constexpr std::string_view REFERENCE_HELD_QUERY = R"sql(
SELECT EXISTS (SELECT 1 FROM wallet_movements
               WHERE user_id = :user_id AND reason = :reason
                 AND reference = :reference)
)sql";

/** @brief The statements of the table `owned_looks`. */
constexpr std::string_view INSERT_OWNED_LOOK_STATEMENT = R"sql(
INSERT INTO owned_looks (user_id, item, origin, acquired_at)
VALUES (:user_id, :item, :origin, :acquired_at)
ON CONFLICT (user_id, item) DO NOTHING
)sql";
constexpr std::string_view OWNED_ITEMS_QUERY =
    "SELECT item FROM owned_looks WHERE user_id = :user_id";
constexpr std::string_view OWNS_LOOK_QUERY = R"sql(
SELECT EXISTS (SELECT 1 FROM owned_looks
               WHERE user_id = :user_id AND item = :item)
)sql";

/** @brief The statement of the table `credited_results`. */
constexpr std::string_view INSERT_CREDITED_RESULT_STATEMENT = R"sql(
INSERT INTO credited_results (reference, user_id, credited_at)
VALUES (:reference, :user_id, :credited_at)
ON CONFLICT (reference) DO NOTHING
)sql";

/** @brief The parameters of the statements of the wallets. */
constexpr std::string_view WALLET_USER_ID_PARAMETER = ":user_id";
constexpr std::string_view KOPECKS_PARAMETER = ":kopecks";
constexpr std::string_view CHIPS_PARAMETER = ":chips";
constexpr std::string_view OPENED_AT_PARAMETER = ":opened_at";
constexpr std::string_view MOVEMENT_ID_PARAMETER = ":id";
constexpr std::string_view CURRENCY_PARAMETER = ":currency";
constexpr std::string_view AMOUNT_PARAMETER = ":amount";
constexpr std::string_view REASON_PARAMETER = ":reason";
constexpr std::string_view REFERENCE_PARAMETER = ":reference";
constexpr std::string_view MADE_AT_PARAMETER = ":made_at";
constexpr std::string_view ITEM_PARAMETER = ":item";
constexpr std::string_view ORIGIN_PARAMETER = ":origin";
constexpr std::string_view ACQUIRED_AT_PARAMETER = ":acquired_at";
constexpr std::string_view CREDITED_AT_PARAMETER = ":credited_at";

/** @brief The columns the queries of the wallets read. */
constexpr int KOPECKS_COLUMN = 0;
constexpr int CHIPS_COLUMN = 1;
constexpr int FIRST_COLUMN = 0;
constexpr int MOVEMENT_ID_COLUMN = 0;
constexpr int MOVEMENT_CURRENCY_COLUMN = 1;
constexpr int MOVEMENT_AMOUNT_COLUMN = 2;
constexpr int MOVEMENT_REASON_COLUMN = 3;
constexpr int MOVEMENT_MADE_AT_COLUMN = 4;

/** @brief How each LookOrigin is written, in the order of the enum. */
constexpr std::array<std::string_view, 3> LOOK_ORIGIN_TEXTS{"starter",
                                                            "purchase", "game"};

}  // namespace rtype::master
