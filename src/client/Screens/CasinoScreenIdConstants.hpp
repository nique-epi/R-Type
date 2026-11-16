#pragma once

#include "ScreenId.hpp"

namespace rtype::client {

/** @brief The casino, pushed over the waiting room: the front of the Bydo
 * Palace and its butler, then, once the player entered, the hall. */
constexpr ScreenId CASINO_SCREEN{"casino"};

/** @brief The cashier of the casino, pushed over the hall by BUY CHIPS: it
 * changes kopecks into chips. */
constexpr ScreenId CASINO_BUY_CHIPS_SCREEN{"casino buy chips"};

/** @brief The cashier of the casino, pushed over the hall by CASH OUT: it
 * changes chips back into kopecks. */
constexpr ScreenId CASINO_CASH_OUT_SCREEN{"casino cash out"};

/** @brief The roulette table, pushed over the hall by ROULETTE. */
constexpr ScreenId ROULETTE_SCREEN{"roulette"};

/** @brief The name of the card of the destination screen that opens the
 * casino. */
constexpr const char* CASINO_LABEL = "CASINO";

}  // namespace rtype::client
