#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "Currency.hpp"
#include "Wallet.hpp"

namespace rtype::master {

/** @returns The balance of @p wallet in @p currency. */
[[nodiscard]] std::uint32_t balanceIn(const network::Wallet& wallet,
                                      network::Currency currency);

/** @returns @p wallet with @p balance as its balance in @p currency. */
[[nodiscard]] network::Wallet withBalance(network::Wallet wallet,
                                          network::Currency currency,
                                          std::uint32_t balance);

/** @returns The largest balance of @p currency: network::MAXIMUM_KOPECKS or
 * network::MAXIMUM_CHIPS. */
[[nodiscard]] std::uint32_t maximumOf(network::Currency currency);

/** @returns The currency the cashier changes @p currency into. */
[[nodiscard]] network::Currency otherCurrency(network::Currency currency);

/** @returns Whether @p wallet holds @p amount or more in @p currency. */
[[nodiscard]] bool canSpend(const network::Wallet& wallet,
                            network::Currency currency, std::uint32_t amount);

/** @returns Whether @p amount more in @p currency leaves @p wallet at its
 * largest balance or under it. */
[[nodiscard]] bool canReceive(const network::Wallet& wallet,
                              network::Currency currency, std::uint32_t amount);

/** @returns The balance of @p wallet in @p currency once it gains @p gain,
 * the largest balance of @p currency at most. */
[[nodiscard]] std::uint32_t creditedBalance(const network::Wallet& wallet,
                                            network::Currency currency,
                                            std::uint64_t gain);

/** @returns @p items, names of looks, in the order of
 * identifiers::SHIP_LOOK_ITEM_NAMES; a name of no look goes last. */
[[nodiscard]] std::vector<std::string> inCatalogueOrder(
    std::vector<std::string> items);

}  // namespace rtype::master
