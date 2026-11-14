#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "AnnouncedBoss.hpp"
#include "ILevelBosses.hpp"

namespace rtype::game::testing {

/** @brief Bosses of a level that only record what a stage announces: the
 * names it knows as bosses are given, and the bosses stay pending until the
 * test lets them enter. */
class RecordingLevelBosses : public ILevelBosses {
 public:
  explicit RecordingLevelBosses(std::vector<std::string> bosses);

  [[nodiscard]] bool isBoss(std::string_view name) const override;
  void announce(std::string_view name, int healthPercent) override;
  [[nodiscard]] std::size_t pending() const override;

  /** @brief Lets every boss announced so far enter. */
  void enterAll();

  /** @returns Every boss announced, in order. */
  [[nodiscard]] std::span<const AnnouncedBoss> announced() const;

 private:
  std::vector<std::string> bosses_;
  std::vector<AnnouncedBoss> announced_;
  std::size_t entered_{0};
};

}  // namespace rtype::game::testing
