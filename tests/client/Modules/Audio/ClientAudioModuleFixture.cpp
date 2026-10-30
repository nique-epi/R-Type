#include "ClientAudioModuleFixture.hpp"
#include <string>
#include "ClientConstants.hpp"
#include "InstallContext.hpp"
#include "LoopStages.hpp"
#include "ModuleConstants.hpp"
#include "ServiceRegistry.hpp"

ClientAudioModuleFixture::ClientAudioModuleFixture()
    : library_(folder_.path()), stages_(rtype::client::CLIENT_LOOP_STAGES) {
  services_.provide(library_);
  services_.provide(settings_);
}

rtype::engine::InstallContext ClientAudioModuleFixture::context() {
  return {std::string{rtype::client::AUDIO_MODULE_NAME}, services_, stages_};
}

rtype::engine::InstallContext
ClientAudioModuleFixture::contextWithoutServices() {
  return {std::string{rtype::client::AUDIO_MODULE_NAME}, noServices_, stages_};
}

rtype::engine::ServiceRegistry& ClientAudioModuleFixture::services() {
  return services_;
}

rtype::engine::LoopStages& ClientAudioModuleFixture::stages() {
  return stages_;
}
