#pragma once

/** @brief What a RecordingModule writes in its journal before its name, for
 * each call. */
constexpr const char* INSTALL_CALL = "install ";
constexpr const char* START_CALL = "start ";
constexpr const char* STOP_CALL = "stop ";

/** @brief The message of the StartRefusedException a RecordingModule throws. */
constexpr const char* START_REFUSAL_REASON = "the test refuses this start";
