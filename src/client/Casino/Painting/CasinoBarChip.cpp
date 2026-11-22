#include "CasinoBarChip.hpp"
#include <string>
#include <utility>
#include "CasinoBar.hpp"
#include "CasinoChipWiring.hpp"
#include "KeyChip.hpp"
#include "KeyChipParts.hpp"

namespace rtype::client {

KeyChip chipOf(const CasinoBar& bar, CasinoChipWiring wiring) {
  return KeyChip(
      KeyChipParts{.text = {.label = std::string(bar.label.in(wiring.language)),
                            .keyName = bar.keyName},
                   .area = bar.area,
                   .key = bar.key,
                   .action = std::move(wiring.action),
                   .sounds = wiring.sounds,
                   .restState = bar.restState});
}

}  // namespace rtype::client
