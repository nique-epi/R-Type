#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include "PurchaseRefusal.hpp"
#include "Purse.hpp"

namespace rtype::client {

/** @returns The status line of the chosen ware @p item: its name, then its
 * price and how to buy it, or that @p purse already owns it. */
[[nodiscard]] std::string wareLineOf(std::size_t item, const Purse& purse);

/** @returns The status line of a purchase of @p item that went through. */
[[nodiscard]] std::string boughtLineOf(std::size_t item);

/** @returns The status line of a purchase refused for @p refusal;
 * @p missing is how many kopecks a too expensive ware lacks. */
[[nodiscard]] std::string refusalLineOf(PurchaseRefusal refusal,
                                        std::uint32_t missing);

}  // namespace rtype::client
