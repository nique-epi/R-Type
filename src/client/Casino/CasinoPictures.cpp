#include "CasinoPictures.hpp"
#include <SFML/Graphics/Image.hpp>
#include "AssetLibrary.hpp"
#include "CashierRoom.hpp"
#include "CashierRoomScan.hpp"
#include "CasinoSpriteConstants.hpp"
#include "EntranceLightScan.hpp"
#include "EntranceLights.hpp"
#include "HallPicture.hpp"
#include "HallPictureScan.hpp"

namespace rtype::client {

namespace {

/** @returns The picture of @p assetId, read from its file in @p assets; an
 * empty one when it cannot be read. */
sf::Image pictureOf(const AssetLibrary& assets, const char* assetId) {
  sf::Image picture;
  if (!picture.loadFromFile(assets.fileOf(assetId))) {
    return sf::Image{};
  }
  return picture;
}

}  // namespace

const EntranceLights& CasinoPictures::front(const AssetLibrary& assets) {
  if (!front_.has_value()) {
    front_ = entranceLightsOf(pictureOf(assets, CASINO_DOORS_CLOSED_ID));
  }
  return *front_;
}

const HallPicture& CasinoPictures::hall(const AssetLibrary& assets) {
  if (!hall_.has_value()) {
    hall_ = hallPictureOf(pictureOf(assets, HALL_PICTURE_ID));
  }
  return *hall_;
}

const CashierRoom& CasinoPictures::cage(const AssetLibrary& assets) {
  if (!cage_.has_value()) {
    cage_ = cashierRoomOf(pictureOf(assets, CASHIER_ROOM_ID));
  }
  return *cage_;
}

}  // namespace rtype::client
