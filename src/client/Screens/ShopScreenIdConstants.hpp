#pragma once

#include "ScreenId.hpp"

namespace rtype::client {

/** @brief The shop, pushed over the destination screen by SHOP: the looks
 * and the hulls the player buys with kopecks. */
constexpr ScreenId SHOP_SCREEN{"shop"};

/** @brief The name of the card of the destination screen that opens the
 * shop. */
constexpr const char* SHOP_LABEL = "SHOP";

}  // namespace rtype::client
