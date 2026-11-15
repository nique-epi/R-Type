#include "PlayerPurse.hpp"
#include "PlayerSettings.hpp"
#include "Purse.hpp"

namespace rtype::client {

Purse purseOf(const PlayerSettings& settings) {
  return Purse{.kopecks = settings.kopecks,
               .chips = settings.chips,
               .locker = settings.shipLocker};
}

}  // namespace rtype::client
