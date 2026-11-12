#pragma once

#include <cstddef>
#include "ShopkeeperEye.hpp"

namespace rtype::client {

/** @brief A change of the eye of the shopkeeper: from the step @c step of
 * the cycle of his eye, it shows @c eye. */
struct ShopkeeperEyeCue {
  std::size_t step{};
  ShopkeeperEye eye{};
};

}  // namespace rtype::client
