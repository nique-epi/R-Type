#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include "IEntityOrders.hpp"
#include "Position.hpp"

/** @brief A body a script asked for, its sheet name copied out of the
 * script. */
struct RecordedSpawn {
  std::string type;
  rtype::game::Position position{};
};

/** @brief Body orders that record what the scripts order and give every body
 * the same network id. */
class RecordingEntityOrders final : public rtype::game::IEntityOrders {
 public:
  explicit RecordingEntityOrders(std::uint32_t givenId);

  [[nodiscard]] std::uint32_t spawn(std::string_view type,
                                    rtype::game::Position position) override;
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
