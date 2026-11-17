#pragma once

#include <cstdint>
#include <string_view>
#include "IEntityOrders.hpp"
#include "Position.hpp"
#include "SpawnOptions.hpp"

/** @brief Body orders that refuse every body, as the game does past the
 * bodies of a tick, and remove nothing. */
class RefusingEntityOrders final : public rtype::game::IEntityOrders {
 public:
  /** @throws rtype::engine::ScriptException every time. */
  [[nodiscard]] std::uint32_t spawn(std::string_view type,
                                    rtype::game::Position position,
                                    rtype::game::SpawnOptions options) override;
  void destroy(std::uint32_t networkId) override;
};
