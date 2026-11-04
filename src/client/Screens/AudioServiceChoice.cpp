#include "AudioServiceChoice.hpp"
#include "IAudioService.hpp"
#include "InstallContext.hpp"

namespace rtype::client {

IAudioService& chooseAudioService(const engine::InstallContext& context,
                                  IAudioService& silence) {
  auto* const provided = context.find<IAudioService>();
  return provided != nullptr ? *provided : silence;
}

}  // namespace rtype::client
