#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace rtype::engine {

/**
 * @brief The value of a declared setting, as the part of the program that
 * declared it reads it.
 *
 * The reference a declaration returns stays valid as long as the registry
 * lives, and shows the newest value `set` kept; only the registry writes it,
 * and only after checking the value.
 */
class SettingValue {
 public:
  /** @returns The whole number; none for a text setting, or for a number
   * setting cleared by `none`. */
  [[nodiscard]] std::optional<std::int64_t> integer() const;

  /** @returns The text; empty for a number setting. */
  [[nodiscard]] const std::string& text() const;

 private:
  friend class ConfigurationRegistry;

  SettingValue(std::optional<std::int64_t> integer, std::string text);

  std::optional<std::int64_t> integer_;
  std::string text_;
};

}  // namespace rtype::engine
