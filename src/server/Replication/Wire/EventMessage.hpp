#pragma once

#include "EncodedMessage.hpp"

namespace rtype::server {

/**
 * @brief The message an event of the match is written as, and how it reaches
 * the players: sent once when a later state message corrects its loss,
 * guaranteed when no state message repeats what it shows.
 */
struct EventMessage {
  network::EncodedMessage message;
  bool guaranteed{false};
};

}  // namespace rtype::server
