#include "StartRefusedException.hpp"
#include <stdexcept>
#include "RecordingModuleConstants.hpp"

StartRefusedException::StartRefusedException()
    : std::runtime_error(START_REFUSAL_REASON) {}
