#include "FakeEntityMessageSource.hpp"
#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>
#include "EntityMessage.hpp"

void FakeEntityMessageSource::queue(rtype::client::EntityMessage message) {
  queued_.push_back(std::move(message));
}

void FakeEntityMessageSource::drainEntityMessages(
    std::vector<rtype::client::EntityMessage>& destination) {
  std::ranges::move(queued_, std::back_inserter(destination));
  queued_.clear();
}
