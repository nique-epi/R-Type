#pragma once

#include <string_view>
#include "Setting.hpp"

namespace rtype::client {

/** @brief An integer setting and its name in the file. */
struct NamedNumber {
  std::string_view name;
  const Setting<int>* setting = nullptr;
};

}  // namespace rtype::client
