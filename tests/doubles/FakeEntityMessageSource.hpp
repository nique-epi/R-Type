#pragma once

#include <vector>
#include "EntityMessage.hpp"
#include "IEntityMessageSource.hpp"

/**
 * @brief Entity message source double: the next drain hands over the messages
 * the test queued, in the order it queued them, as ServerConnection hands over
 * those of the server in the order they arrived. A test may queue what the
 * network brings: the copies of a message, the parts of one state.
 */
class FakeEntityMessageSource : public rtype::client::IEntityMessageSource {
 public:
  /** @brief Queues @p message for the next drain. */
  void queue(rtype::client::EntityMessage message);

  void drainEntityMessages(
      std::vector<rtype::client::EntityMessage>& destination) override;

 private:
  std::vector<rtype::client::EntityMessage> queued_;
};
