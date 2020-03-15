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
