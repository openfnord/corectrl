// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/idatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <fcntl.h>
#include <filesystem>
#include <functional>
#include <string>
#include <unistd.h>
#include <utility>

template<typename T>
class DevFSDataSource : public IDataSource<T>
{
 public:
  DevFSDataSource(std::filesystem::path const &path,
                  std::function<T(int)> &&reader) noexcept
  : path_(path.string())
  , reader_(std::move(reader))
  {
    fd_ = open(path.c_str(), O_RDONLY);
    if (fd_ < 0)
      LOG(ERROR) << fmt::format("Cannot open {}", path.c_str());
  }

  ~DevFSDataSource() override
  {
    if (fd_ > 0)
      close(fd_);
  }

  std::string source() const override
  {
    return path_;
  }

  bool read(T &data) override
  {
    data = reader_(fd_);
    return true;
  }

 private:
  std::string const path_;
  std::function<T(int)> const reader_;
  int fd_;
};
