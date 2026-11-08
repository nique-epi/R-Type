#include "WonItems.hpp"
#include <cstdint>
#include <vector>
#include "IShipLookExchange.hpp"
#include "SettingDefinitionConstants.hpp"
#include "SettingsService.hpp"
#include "ShipLocker.hpp"
#include "ShipLookNumbers.hpp"

namespace rtype::client {

void storeWonItems(IShipLookExchange& shipLooks, SettingsService& settings) {
  const std::vector<std::uint8_t> won = shipLooks.takeWonItems();
  if (won.empty()) {
    return;
  }
  ShipLocker locker = settings.settings().shipLocker;
  for (const std::uint8_t item : won) {
    locker.items.set(identifiers::lookIndexOf(item));
  }
  static_cast<void>(settings.apply(SHIP_LOCKER_SETTING, locker));
  static_cast<void>(settings.saveIfChanged());
}

}  // namespace rtype::client
