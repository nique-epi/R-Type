#include "SoundBank.hpp"
#include <SFML/Audio/SoundBuffer.hpp>
#include <span>
#include <string_view>
#include "AssetLibrary.hpp"
#include "AssetProblemReport.hpp"
#include "AssetsById.hpp"

namespace rtype::client {

void SoundBank::load(const AssetLibrary& library,
                     std::span<const char* const> soundIds) {
  AssetProblemReport problems(library);
  for (const char* soundId : soundIds) {
    problems.record(soundId,
                    loadAssetFile(sounds_, soundId, library.fileOf(soundId)));
  }
  problems.throwIfAny();
}

bool SoundBank::isEmpty() const { return sounds_.empty(); }

const sf::SoundBuffer& SoundBank::sound(std::string_view soundId) const {
  return loadedAsset(sounds_, soundId);
}

}  // namespace rtype::client
