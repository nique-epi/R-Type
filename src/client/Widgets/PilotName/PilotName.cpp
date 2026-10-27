#include "PilotName.hpp"
#include <string>
#include <string_view>
#include "WidgetText.hpp"

namespace rtype::client {

std::string pilotNameOf(std::string_view nickname, int playerNumber) {
  return nickname.empty() ? playerNumberLabel(playerNumber)
                          : std::string{nickname};
}

}  // namespace rtype::client
