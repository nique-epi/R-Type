#pragma once

#include <optional>
#include <string>
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The stamp the options write after a save: SAVED for
 * SAVED_STAMP_DURATION, NOT SAVED until the next save after a failure.
 */
class SavedStamp {
 public:
  /** @brief Writes SAVED, for SAVED_STAMP_DURATION from now. */
  void markSaved();

  /** @brief Writes NOT SAVED, which stays until the next save. */
  void markNotSaved();

  /** @brief Lets @p elapsed pass: SAVED is erased once it has been written for
   * SAVED_STAMP_DURATION. */
  void age(engine::Duration elapsed);

  /** @returns The stamp to write, empty when there is none. */
  [[nodiscard]] const std::string& text() const;

 private:
  std::string text_;
  /** @brief How long SAVED has been written; nothing while no stamp fades. */
  std::optional<engine::Duration> age_;
};

}  // namespace rtype::client
