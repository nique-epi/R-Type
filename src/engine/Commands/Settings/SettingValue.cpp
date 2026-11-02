#include "SettingValue.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <utility>

namespace rtype::engine {

SettingValue::SettingValue(std::optional<std::int64_t> integer,
                           std::string text)
    : integer_(integer), text_(std::move(text)) {}

std::optional<std::int64_t> SettingValue::integer() const { return integer_; }

const std::string& SettingValue::text() const { return text_; }

}  // namespace rtype::engine
