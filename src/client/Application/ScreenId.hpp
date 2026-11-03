#pragma once

#include <string_view>

namespace rtype::client {

/**
 * @brief The name a screen is registered under, and asked for by.
 *
 * Every module declares the names of its own screens in a constants header of
 * its own, so adding a screen touches no list shared by the modules. A name is
 * compared by its text, and must outlive the identifier: the names of the
 * constants headers are string literals.
 */
class ScreenId {
 public:
  explicit constexpr ScreenId(std::string_view name) : name_(name) {}

  /** @returns The text the screen is registered under. */
  [[nodiscard]] constexpr std::string_view name() const { return name_; }

  constexpr auto operator<=>(const ScreenId& other) const = default;

 private:
  std::string_view name_;
};

}  // namespace rtype::client
