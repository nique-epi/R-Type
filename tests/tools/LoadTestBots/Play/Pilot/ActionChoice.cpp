#include "ActionChoice.hpp"
#include <cstddef>
#include <optional>
#include "ActionChoiceConstants.hpp"
#include "ActionSet.hpp"
#include "PlayerAction.hpp"

namespace rtype::tools {

client::ActionSet actionSetOf(std::size_t choice) {
  client::ActionSet held;
  for (const std::optional<client::PlayerAction>& action :
       DIRECTION_CHOICES.at(choice % DIRECTION_CHOICES.size())) {
    if (action.has_value()) {
      held.add(*action);
    }
  }
  if (choice >= DIRECTION_CHOICES.size()) {
    held.add(client::PlayerAction::Shoot);
  }
  return held;
}

}  // namespace rtype::tools
