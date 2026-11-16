#pragma once

#include <cstdint>
#include <string>
#include "CashierExchange.hpp"
#include "CashierWay.hpp"
#include "ExchangeRequest.hpp"
#include "Wallet.hpp"
#include "WalletRefusal.hpp"

namespace rtype::client {

/** @returns The request that changes @p amount @p way: out of the kopecks to
 * buy chips, out of the chips to cash out. */
[[nodiscard]] network::ExchangeRequest exchangeRequestOf(CashierWay way,
                                                         std::uint32_t amount);

/** @returns What changing @p amount of @p wallet @p way gives, read with the
 * rules of the master: exchangeRefusalOf(), then exchangedWallet() when it
 * accepts. */
[[nodiscard]] CashierExchange exchangeAt(const network::Wallet& wallet,
                                         CashierWay way, std::uint32_t amount);

/** @returns The balance @p way takes from in @p wallet: the kopecks to buy
 * chips, the chips to cash out. */
[[nodiscard]] std::uint32_t sourceOf(const network::Wallet& wallet,
                                     CashierWay way);

/** @returns The balance @p way gives to in @p wallet. */
[[nodiscard]] std::uint32_t destinationOf(const network::Wallet& wallet,
                                          CashierWay way);

/**
 * @returns Why @p tried, an exchange of @p amount @p way, was refused, in
 * words: CHOOSE AN AMOUNT; NOT ENOUGH KOPECKS (or CHIPS), with how many are
 * missing; NO ROOM LEFT FOR MORE CHIPS (or KOPECKS). Nothing for an exchange
 * the cashier makes.
 */
[[nodiscard]] std::string cashierLineOf(const CashierExchange& tried,
                                        CashierWay way, std::uint32_t amount);

/** @returns Why the master did not make an exchange, in words. */
[[nodiscard]] std::string cashierRefusalLineOf(WalletRefusal refusal);

}  // namespace rtype::client
