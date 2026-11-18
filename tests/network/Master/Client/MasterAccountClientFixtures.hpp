#pragma once

#include "ExchangeReceipt.hpp"
#include "OwnedLooks.hpp"
#include "User.hpp"

/** @returns The account of the token: ACE_USER_ID, named ACE_USER_NICKNAME, a
 * player. */
[[nodiscard]] rtype::network::User aceUser();

/** @returns The looks it owns: the starter kit and the crown. */
[[nodiscard]] rtype::network::OwnedLooks aceLooks();

/** @returns The receipt of the cashier for BUY_500_CHIPS: the exchange
 * BUY_500_CHIPS_ID, made at BUY_500_CHIPS_MADE_AT, and WALLET_AFTER. */
[[nodiscard]] rtype::network::ExchangeReceipt buyFiveHundredChipsReceipt();
