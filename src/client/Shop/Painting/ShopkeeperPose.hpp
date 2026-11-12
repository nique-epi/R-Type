#pragma once

#include "ShopkeeperEye.hpp"

namespace rtype::client {

/** @brief How the shopkeeper stands at a moment: how many pixels under his
 * place of rest he is drawn (above it when negative), and his eye. */
struct ShopkeeperPose {
  int drop{};
  ShopkeeperEye eye{};
};

}  // namespace rtype::client
