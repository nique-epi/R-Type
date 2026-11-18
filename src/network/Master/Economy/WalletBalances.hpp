#pragma once

#include <cstdint>
#include <optional>
#include "Currency.hpp"
#include "ExchangeRequest.hpp"
#include "Wallet.hpp"
#include "WalletExchangeRefusal.hpp"

namespace rtype::network {

/** @returns The balance of @p wallet in @p currency. */
[[nodiscard]] std::uint32_t balanceIn(const Wallet& wallet, Currency currency);

/** @returns @p wallet with @p balance as its balance in @p currency. */
[[nodiscard]] Wallet withBalance(Wallet wallet, Currency currency,
                                 std::uint32_t balance);

/** @returns The largest balance of @p currency: MAXIMUM_KOPECKS or
 * MAXIMUM_CHIPS. */
[[nodiscard]] std::uint32_t maximumOf(Currency currency);

/** @returns The currency the cashier changes @p currency into. */
[[nodiscard]] Currency otherCurrency(Currency currency);

/** @returns Whether @p wallet holds @p amount or more in @p currency. */
[[nodiscard]] bool canSpend(const Wallet& wallet, Currency currency,
                            std::uint32_t amount);

/** @returns Whether @p amount more in @p currency leaves @p wallet at its
 * largest balance or under it. */
[[nodiscard]] bool canReceive(const Wallet& wallet, Currency currency,
                              std::uint32_t amount);

/** @returns The balance of @p wallet in @p currency once it gains @p gain,
 * the largest balance of @p currency at most. */
[[nodiscard]] std::uint32_t creditedBalance(const Wallet& wallet,
                                            Currency currency,
                                            std::uint64_t gain);

/** @returns Why the cashier refuses @p request from a wallet of @p before,
 * nothing when it accepts it: InvalidAmount for an amount of none or past the
 * largest balance, NotEnough when @p before holds less, PastTheMaximum when
 * the other balance would pass its largest value. */
[[nodiscard]] std::optional<WalletExchangeRefusal> exchangeRefusalOf(
    const Wallet& before, const ExchangeRequest& request);

/** @returns The balances of @p before once the amount of @p request changed
 * currency, one for one; @p request is one exchangeRefusalOf() accepts. */
[[nodiscard]] Wallet exchangedWallet(const Wallet& before,
                                     const ExchangeRequest& request);

}  // namespace rtype::network
