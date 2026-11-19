#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "IMapBallot.hpp"

/**
 * @brief Map ballot double that writes down every vote asked for, in order,
 * and answers the last one as the map requested, as a real ballot does.
 */
class RecordingMapBallot : public rtype::client::IMapBallot {
 public:
  /** @brief Writes the vote down. */
  void voteFor(std::uint8_t map) override;
  [[nodiscard]] std::optional<std::uint8_t> requestedMap() const override;

  /** @returns The maps voteFor() was called with, in order. */
  [[nodiscard]] const std::vector<std::uint8_t>& votes() const;

 private:
  std::vector<std::uint8_t> votes_;
};
