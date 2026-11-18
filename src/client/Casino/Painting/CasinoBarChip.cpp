#include "CasinoBarChip.hpp"
#include <functional>
#include <utility>
#include "CasinoBar.hpp"
#include "ISoundPlayer.hpp"
#include "KeyChip.hpp"
#include "KeyChipParts.hpp"

namespace rtype::client {

KeyChip chipOf(const CasinoBar& bar, std::function<void()> action,
               ISoundPlayer& sounds) {
  return KeyChip(
      KeyChipParts{.text = {.label = bar.label, .keyName = bar.keyName},
                   .area = bar.area,
                   .key = bar.key,
                   .action = std::move(action),
                   .sounds = sounds,
                   .restState = bar.restState});
}

}  // namespace rtype::client
