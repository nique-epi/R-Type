#pragma once

#include <cstdint>
#include <string>
#include "CashierWay.hpp"
#include "Exchange.hpp"
#include "Purse.hpp"
#include "WalletRefusal.hpp"

namespace rtype::client {

/** @returns What changing @p amount of @p purse @p way gives: buyChips() or
 * cashOutChips(). */
[[nodiscard]] Exchange exchangeAt(const Purse& purse, CashierWay way,
                                  std::uint32_t amount);

/** @returns The balance @p way takes from in @p purse: the kopecks to buy
 * chips, the chips to cash out. */
[[nodiscard]] std::uint32_t sourceOf(const Purse& purse, CashierWay way);

/** @returns The balance @p way gives to in @p purse. */
[[nodiscard]] std::uint32_t destinationOf(const Purse& purse, CashierWay way);

/**
 * @returns Why @p tried, an exchange of @p amount @p way, was refused, in
 * words: CHOOSE AN AMOUNT; NOT ENOUGH KOPECKS (or CHIPS), with how many are
 * missing; NO ROOM LEFT FOR MORE CHIPS (or KOPECKS). Nothing for an exchange
 * the cashier makes.
 */
[[nodiscard]] std::string cashierLineOf(const Exchange& tried, CashierWay way,
                                        std::uint32_t amount);

/** @returns Why the master did not make an exchange, in words. */
[[nodiscard]] std::string cashierRefusalLineOf(WalletRefusal refusal);

}  // namespace rtype::client
