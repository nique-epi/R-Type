#pragma once

#include "Menu.hpp"
#include "RecordingRowControl.hpp"

/** @returns A dialog of two recording rows, then BACK and APPLY. */
[[nodiscard]] rtype::client::Menu dialogWithRows(RowControlRecord& first,
                                                 RowControlRecord& second);
