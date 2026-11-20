#pragma once

#include "ScreenId.hpp"

namespace rtype::client {

/** @brief The casino, pushed over the server map: the front of the Bydo
 * Palace and its butler, its doors closed. */
constexpr ScreenId CASINO_SCREEN{"casino"};

/** @brief The main hall of the casino, pushed over its doors once the player
 * entered: the boxing ring, the way to the cashier and to the roulette. */
constexpr ScreenId CASINO_HALL_SCREEN{"casino hall"};

/** @brief The cashier of the casino, pushed over the hall by CASHIER: it
 * changes kopecks into chips, or chips back into kopecks. */
constexpr ScreenId CASINO_CASHIER_SCREEN{"casino cashier"};

/** @brief The roulette table, pushed over the hall by ROULETTE. */
constexpr ScreenId ROULETTE_SCREEN{"roulette"};

}  // namespace rtype::client
