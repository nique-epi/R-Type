#include "SoundBank.hpp"
#include <SFML/Audio/SoundBuffer.hpp>
#include <filesystem>
#include <span>
#include <string_view>
#include "AssetLibrary.hpp"
#include "AssetLoadAttempt.hpp"
#include "AssetProblemRecording.hpp"
#include "AssetsById.hpp"
#include "ClientException.hpp"
#include "PathText.hpp"

namespace rtype::client {

void SoundBank::load(const AssetLibrary& library,
                     std::span<const char* const> soundIds) {
  AssetProblems problems;
  for (const char* soundId : soundIds) {
    const std::filesystem::path file = library.fileOf(soundId);
    recordProblem(problems, AssetLoadAttempt{.assetId = soundId,
                                             .file = file,
                                             .result = loadAssetFile(
                                                 sounds_, soundId, file)});
  }
  if (hasProblems(problems)) {
    throw AssetLoadingException(engine::genericText(library.folder()),
                                problems);
  }
}

const sf::SoundBuffer& SoundBank::sound(std::string_view soundId) const {
  return loadedAsset(sounds_, soundId);
}

}  // namespace rtype::client
