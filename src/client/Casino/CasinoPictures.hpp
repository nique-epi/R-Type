#pragma once

#include <optional>
#include "AssetLibrary.hpp"
#include "CashierRoom.hpp"
#include "EntranceLights.hpp"
#include "HallPicture.hpp"

namespace rtype::client {

/**
 * @brief What the rooms of the casino read in their pictures, read once, the
 * first time each room opens, then kept: the lights of the front, the life of
 * the hall and the cage of the cashier. A picture that cannot be read gives a
 * room that stays still.
 */
class CasinoPictures {
 public:
  /** @returns The lights of the front, read in CASINO_DOORS_CLOSED_ID. */
  [[nodiscard]] const EntranceLights& front(const AssetLibrary& assets);
  /** @returns The life of the hall, read in HALL_PICTURE_ID. */
  [[nodiscard]] const HallPicture& hall(const AssetLibrary& assets);
  /** @returns The cage of the cashier, read in CASHIER_ROOM_ID. */
  [[nodiscard]] const CashierRoom& cage(const AssetLibrary& assets);

 private:
  std::optional<EntranceLights> front_;
  std::optional<HallPicture> hall_;
  std::optional<CashierRoom> cage_;
};

}  // namespace rtype::client
