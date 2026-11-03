#pragma once

#include <string_view>
#include "ISoundPlayer.hpp"

namespace rtype::client {

/** @brief The sound player of a client without sound: plays nothing, for any
 * id, and never fails. */
class NullSoundPlayer final : public ISoundPlayer {
 public:
  void play(std::string_view soundId) override;
};

}  // namespace rtype::client
