#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "IScriptWorld.hpp"
#include "Position.hpp"
#include "ScriptedSpawn.hpp"

/** @brief A body a script asked for, its sheet name copied out of the
 * script. */
struct RecordedSpawn {
  std::string type;
  rtype::game::Position position{};
};

/** @brief A script world that records what the scripts ask and gives every
 * body it is asked for the same network id. */
class RecordingScriptWorld final : public rtype::game::IScriptWorld {
 public:
  explicit RecordingScriptWorld(std::uint32_t givenId);

  [[nodiscard]] std::uint32_t spawn(
      const rtype::game::ScriptedSpawn& spawn) override;
  void destroy(std::uint32_t networkId) override;

  /** @returns The bodies asked for, in order. */
  [[nodiscard]] const std::vector<RecordedSpawn>& spawns() const;
  /** @returns The network ids asked to be removed, in order. */
  [[nodiscard]] const std::vector<std::uint32_t>& removals() const;

 private:
  std::uint32_t givenId_;
  std::vector<RecordedSpawn> spawns_;
  std::vector<std::uint32_t> removals_;
};
