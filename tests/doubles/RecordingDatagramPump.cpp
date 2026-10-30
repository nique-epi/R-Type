#include "RecordingDatagramPump.hpp"
#include <cstddef>

void RecordingDatagramPump::pump() { ++pumpCount_; }

std::size_t RecordingDatagramPump::pumpCount() const { return pumpCount_; }
