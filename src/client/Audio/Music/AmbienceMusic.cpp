#include "AmbienceMusic.hpp"
#include <array>
#include <vector>
#include "Ambience.hpp"
#include "AssetLibrary.hpp"
#include "AudioConstants.hpp"
#include "MusicPlayer.hpp"
#include "SettingsService.hpp"

namespace rtype::client {

bool replacesTheTheme(Ambience ambience) {
  switch (ambience) {
    case Ambience::None:
    case Ambience::Space:
    case Ambience::Hangar:
      return false;
    case Ambience::Shop:
    case Ambience::ShopRadio:
    case Ambience::ShopDance:
    case Ambience::CasinoFront:
    case Ambience::CasinoHall:
    case Ambience::CasinoTables:
      return true;
  }
  return false;
}

AmbienceMusic::AmbienceMusic(SettingsService& settings)
    : settings_(&settings),
      space_(settings, SPACE_AMBIENCE_TRACK),
      hangar_(settings, HANGAR_AMBIENCE_TRACK),
      hangarCrowd_(settings, CROWD_AMBIENCE_TRACK),
      shop_(settings, SHOP_MUSIC_TRACK),
      shopRadio_(settings, SHOP_RADIO_TRACK),
      shopDance_(settings, SHOP_DANCE_TRACK),
      casinoFront_(settings, CASINO_FRONT_TRACK),
      casinoHall_(settings, CASINO_HALL_TRACK),
      casinoCrowd_(settings, CASINO_CROWD_TRACK),
      casinoTables_(settings, CASINO_TABLES_TRACK) {
  for (MusicPlayer* player : allPlayers()) {
    settings_->subscribe(*player);
  }
}

AmbienceMusic::~AmbienceMusic() {
  for (MusicPlayer* player : allPlayers()) {
    settings_->unsubscribe(*player);
  }
}

void AmbienceMusic::open(AssetLibrary& library) {
  for (MusicPlayer* player : allPlayers()) {
    player->open(library);
  }
}

void AmbienceMusic::play(Ambience ambience) {
  for (MusicPlayer* player : allPlayers()) {
    player->stop();
  }
  for (MusicPlayer* player : playersOf(ambience)) {
    player->start();
  }
}

std::array<MusicPlayer*, AMBIENCE_PLAYER_COUNT> AmbienceMusic::allPlayers() {
  return {&space_,       &hangar_,      &hangarCrowd_, &shop_,
          &shopRadio_,   &shopDance_,   &casinoFront_, &casinoHall_,
          &casinoCrowd_, &casinoTables_};
}

std::vector<MusicPlayer*> AmbienceMusic::playersOf(Ambience ambience) {
  switch (ambience) {
    case Ambience::None:
      return {};
    case Ambience::Space:
      return {&space_};
    case Ambience::Hangar:
      return {&hangar_, &hangarCrowd_};
    case Ambience::Shop:
      return {&shop_};
    case Ambience::ShopRadio:
      return {&shopRadio_};
    case Ambience::ShopDance:
      return {&shopDance_};
    case Ambience::CasinoFront:
      return {&casinoFront_};
    case Ambience::CasinoHall:
      return {&casinoHall_, &casinoCrowd_};
    case Ambience::CasinoTables:
      return {&casinoTables_};
  }
  return {};
}

}  // namespace rtype::client
