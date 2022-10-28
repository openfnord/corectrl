// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/idatasink.h"
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

class ZipDataSink final
: public IDataSink<std::vector<std::pair<std::string, std::vector<char>>>>
{
 public:
  ZipDataSink(std::filesystem::path const &path) noexcept;
  ~ZipDataSink();

  std::string sink() const override;
  bool write(std::vector<std::pair<std::string, std::vector<char>>> const &data)
      override;

 private:
  void backupFile() const;
  void removeBackupFile() const;
  void restorePreWriteFileState() const;

  std::filesystem::path const path_;
};
