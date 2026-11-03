#pragma once

#include <vector>
#include "EntityMessage.hpp"

namespace rtype::client {

/**
 * @brief Where the game takes the entity messages of the server from:
 * EntityMessageQueue in the client, a double in the tests.
 */
class IEntityMessageSource {
 public:
  virtual ~IEntityMessageSource() = default;

  /**
   * @brief Moves every entity message received since the previous call, in
   * the order they arrived, to the end of @p destination. Returns at once when
   * none arrived.
   */
  virtual void drainEntityMessages(std::vector<EntityMessage>& destination) = 0;
};

}  // namespace rtype::client
