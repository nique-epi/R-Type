#include "ChipsExchange.hpp"
#include <cstdint>
#include <optional>
#include "Exchange.hpp"
#include "ExchangeRefusal.hpp"
#include "Purse.hpp"
#include "SettingsConstants.hpp"

namespace rtype::client {

namespace {

/** @brief A way through the cashier: the balance of the purse an exchange
 * takes from, the balance it gives to, why it is refused when the first holds
 * too little, and the largest the second may hold. */
struct ExchangeWay {
  std::uint32_t Purse::* from = nullptr;
  std::uint32_t Purse::* to = nullptr;
  ExchangeRefusal shortOf = ExchangeRefusal::NothingToExchange;
  std::uint32_t largest = 0;
};

/** @returns Why @p purse cannot change @p amount along @p way, the first that
 * holds; nothing when it can. The sum is taken in 64 bits, so a balance near
 * the largest never wraps around. */
std::optional<ExchangeRefusal> refusalOf(const Purse& purse,
                                         std::uint32_t amount,
                                         const ExchangeWay& way) {
  if (amount == 0) {
    return ExchangeRefusal::NothingToExchange;
  }
  if (amount > purse.*way.from) {
    return way.shortOf;
  }
  if (std::uint64_t{purse.*way.to} + amount > way.largest) {
    return ExchangeRefusal::BalanceFull;
  }
  return std::nullopt;
}

/** @returns The change of @p amount along @p way, whole, or the purse as it
 * was and why it was refused. */
Exchange exchange(const Purse& purse, std::uint32_t amount,
                  const ExchangeWay& way) {
  if (const std::optional<ExchangeRefusal> refusal =
          refusalOf(purse, amount, way)) {
    return Exchange{.refusal = refusal, .purse = purse};
  }
  Purse after = purse;
  after.*way.from -= amount;
  after.*way.to += amount;
  return Exchange{.refusal = std::nullopt, .purse = after};
}

}  // namespace

Exchange buyChips(const Purse& purse, std::uint32_t amount) {
  return exchange(purse, amount,
                  ExchangeWay{.from = &Purse::kopecks,
                              .to = &Purse::chips,
                              .shortOf = ExchangeRefusal::NotEnoughKopecks,
                              .largest = MAXIMUM_CHIPS});
}

Exchange cashOutChips(const Purse& purse, std::uint32_t amount) {
  return exchange(purse, amount,
                  ExchangeWay{.from = &Purse::chips,
                              .to = &Purse::kopecks,
                              .shortOf = ExchangeRefusal::NotEnoughChips,
                              .largest = MAXIMUM_KOPECKS});
}

}  // namespace rtype::client
