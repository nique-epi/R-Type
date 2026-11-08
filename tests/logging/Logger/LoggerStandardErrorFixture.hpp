#pragma once

#include "LogLaunchOptionsFixture.hpp"

/** @brief Starts each test at the warn level with no output, keeps what reaches
 * std::cerr, and puts the Logger back afterwards (see
 * LogLaunchOptionsFixture); the tests of the writer of standard error log
 * errors. */
class LoggerStandardErrorFixture : public LogLaunchOptionsFixture {};
