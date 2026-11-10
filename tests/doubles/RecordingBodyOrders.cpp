#include "RecordingBodyOrders.hpp"
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "Position.hpp"

RecordingBodyOrders::RecordingBodyOrders(std::uint32_t givenId)
    : givenId_(givenId) {}

std::uint32_t RecordingBodyOrders::spawn(std::string_view type,
                                         rtype::game::Position position) {
  spawns_.push_back(
      RecordedSpawn{.type = std::string{type}, .position = position});
  return givenId_;
}

void RecordingBodyOrders::destroy(std::uint32_t networkId) {
  removals_.push_back(networkId);
}

const std::vector<RecordedSpawn>& RecordingBodyOrders::spawns() const {
  return spawns_;
}

const std::vector<std::uint32_t>& RecordingBodyOrders::removals() const {
  return removals_;
}
