#pragma once

#include <cstdint>
#include <optional>
#include "IWallet.hpp"
#include "Purse.hpp"

namespace rtype::client {

/** @returns What @p wallet lets the player buy with and already holds: its
 * kopecks, its chips and its looks; nothing until the master answered both
 * the balances and the looks of the account signed in. */
[[nodiscard]] std::optional<Purse> walletPurse(const IWallet& wallet);

/** @returns The kopecks of @p wallet; nothing signed out, or until the master
 * answered them. */
[[nodiscard]] std::optional<std::uint32_t> walletKopecks(const IWallet& wallet);

/** @returns The chips of @p wallet; nothing signed out, or until the master
 * answered them. */
[[nodiscard]] std::optional<std::uint32_t> walletChips(const IWallet& wallet);

}  // namespace rtype::client
