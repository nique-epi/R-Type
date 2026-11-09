#include "RecordingScriptWorld.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include "ScriptedSpawn.hpp"

RecordingScriptWorld::RecordingScriptWorld(std::uint32_t givenId)
    : givenId_(givenId) {}

std::uint32_t RecordingScriptWorld::spawn(
    const rtype::game::ScriptedSpawn& spawn) {
  spawns_.push_back(RecordedSpawn{.type = std::string{spawn.type},
                                  .position = spawn.position});
  return givenId_;
}

void RecordingScriptWorld::destroy(std::uint32_t networkId) {
  removals_.push_back(networkId);
}

const std::vector<RecordedSpawn>& RecordingScriptWorld::spawns() const {
  return spawns_;
}

const std::vector<std::uint32_t>& RecordingScriptWorld::removals() const {
  return removals_;
}
