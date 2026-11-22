#pragma once

#include <array>
#include "CashierConstants.hpp"
#include "CasinoBar.hpp"
#include "CasinoEntranceConstants.hpp"
#include "CasinoHallConstants.hpp"
#include "RouletteScreenConstants.hpp"

namespace rtype::client::testing {

/** @brief Every bar of the casino that writes a label, its area fixed. */
constexpr std::array LABELLED_CASINO_BARS{
    CASHIER_BACK_BAR, BUY_CHIPS_CONFIRM_BAR, CASH_OUT_CONFIRM_BAR, ENTER_BAR,
    DOORS_BACK_BAR,   HALL_CASHIER_BAR,      HALL_ROULETTE_BAR,    CLEAR_BAR,
    REBET_BAR,        TABLE_BACK_BAR};

}  // namespace rtype::client::testing
