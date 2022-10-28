// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "easyloggingpp/easylogging++.h"
#include <filesystem>
#include <string>

void setupLogger(std::filesystem::path const &logFilePath,
                 std::string const &globalLogFlushThreshold = "50")
{
  el::Configurations c;
  c.setToDefault();

  // Global
  c.setGlobally(el::ConfigurationType::Enabled, "true");
  c.setGlobally(el::ConfigurationType::Format,
                "[%datetime{%d-%M-%y %H:%m:%s.%g}][%levshort] %msg");
  c.setGlobally(el::ConfigurationType::ToFile, "true");
  c.setGlobally(el::ConfigurationType::Filename, logFilePath);
  c.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
  c.setGlobally(el::ConfigurationType::SubsecondPrecision, "3");
  c.setGlobally(el::ConfigurationType::PerformanceTracking, "false");
  c.setGlobally(el::ConfigurationType::MaxLogFileSize, "524288");
  c.setGlobally(el::ConfigurationType::LogFlushThreshold,
                globalLogFlushThreshold);

  // Error
  c.set(el::Level::Error, el::ConfigurationType::Format,
        "[%datetime{%d-%M-%y %H:%m:%s.%g}][%levshort] [%fbase:%line] %msg");
  c.set(el::Level::Error, el::ConfigurationType::LogFlushThreshold, "1");

  // Warning
  c.set(el::Level::Warning, el::ConfigurationType::ToStandardOutput, "true");

  el::Loggers::setDefaultConfigurations(c, true);
  el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
}
