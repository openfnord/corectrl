//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// Distributed under the GPL version 3 or any later version.
//
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
