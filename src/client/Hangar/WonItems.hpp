#pragma once

#include "IShipLookExchange.hpp"
#include "SettingsService.hpp"

namespace rtype::client {

/**
 * @brief Puts in the locker every item @p shipLooks says the player won, and
 * saves the settings when one was new. Called once per frame; it does nothing
 * when no item was won.
 */
void storeWonItems(IShipLookExchange& shipLooks, SettingsService& settings);

}  // namespace rtype::client
