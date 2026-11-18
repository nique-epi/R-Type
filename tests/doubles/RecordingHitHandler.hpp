#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "AppliedHit.hpp"
#include "IHitHandler.hpp"

namespace rtype::game::testing {

/** @brief A hit handler of a name a test chooses, which keeps every hit it is
 * told of, oldest first. */
class RecordingHitHandler final : public IHitHandler {
 public:
  explicit RecordingHitHandler(std::string_view name);

  [[nodiscard]] std::string_view name() const override;
  void onHit(const AppliedHit& hit) override;

  /** @returns The hits it was told of, oldest first. */
  [[nodiscard]] const std::vector<AppliedHit>& told() const;

 private:
  std::string name_;
  std::vector<AppliedHit> told_;
};

}  // namespace rtype::game::testing
