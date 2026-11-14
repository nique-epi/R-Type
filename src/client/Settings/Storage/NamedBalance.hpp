#pragma once

#include <cstdint>
#include <string_view>
#include "Setting.hpp"

namespace rtype::client {

/** @brief A balance setting, the kopecks or the chips, and its name in the
 * file. */
struct NamedBalance {
  std::string_view name;
  const Setting<std::uint32_t>* setting = nullptr;
};

}  // namespace rtype::client
