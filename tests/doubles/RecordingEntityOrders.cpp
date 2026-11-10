#include "RecordingEntityOrders.hpp"
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "Position.hpp"

RecordingEntityOrders::RecordingEntityOrders(std::uint32_t givenId)
    : givenId_(givenId) {}

std::uint32_t RecordingEntityOrders::spawn(std::string_view type,
                                           rtype::game::Position position) {
  spawns_.push_back(
      RecordedSpawn{.type = std::string{type}, .position = position});
  return givenId_;
}

void RecordingEntityOrders::destroy(std::uint32_t networkId) {
  removals_.push_back(networkId);
}

const std::vector<RecordedSpawn>& RecordingEntityOrders::spawns() const {
  return spawns_;
}

const std::vector<std::uint32_t>& RecordingEntityOrders::removals() const {
  return removals_;
}
