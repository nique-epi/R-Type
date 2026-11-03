#pragma once

#include <memory>
#include <string>
#include <vector>
#include "AudioMode.hpp"
#include "IAudioService.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"

namespace rtype::client {

/**
 * @brief The module of the sound of the client: the sounds, the theme and the
 * music of a game, which it provides to the modules installed after it as an
 * IAudioService.
 *
 * install() builds the service, the one that plays for AudioMode::Enabled and
 * the one that plays nothing for AudioMode::Disabled, provides it, and adds
 * its work to UPDATE_STAGE, before the screens: the silence before the music
 * of a game moves on. The volumes follow the settings service. start()
 * opens the music files, or writes in the journal that the client runs
 * without sound. Nothing stays open after stop(): the music stops when the
 * module is destroyed, after every screen.
 */
class ClientAudioModule final : public engine::IModule {
 public:
  /** @param mode Whether the client makes sound. */
  explicit ClientAudioModule(AudioMode mode);

  /** @returns AUDIO_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;
  /** @returns The modules whose services the sound is built from: the assets
   * and the settings. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;
  /** @throws MissingServiceException if the assets or the settings module
   * does not provide its service. */
  void install(engine::InstallContext& context) override;
  void start() override;
  void stop() override;

 private:
  AudioMode mode_;
  std::unique_ptr<IAudioService> service_;
  logging::Logger logger_;
};

}  // namespace rtype::client
