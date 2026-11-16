#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include "Currency.hpp"
#include "MovementReason.hpp"

namespace rtype::master {

/** @brief A line the journal of a wallet is given. */
struct MovementRecord {
  /** @brief Its number, chosen by the caller so another line can name it;
   * nothing for the next number of the journal. */
  std::optional<std::int64_t> id;
  std::int64_t userId{};
  network::Currency currency{network::Currency::Kopecks};
  /** @brief What the balance gained, negative for what it lost. */
  std::int64_t amount{};
  network::MovementReason reason{network::MovementReason::Opening};
  /** @brief The fact that moved the balance. */
  std::string reference;
  std::chrono::sys_seconds madeAt;
};

}  // namespace rtype::master
