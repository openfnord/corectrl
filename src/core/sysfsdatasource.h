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
#include "fmt/format.h"
#include "idatasource.h"
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

template<typename T>
class SysFSDataSource : public IDataSource<T>
{
 public:
  SysFSDataSource(
      std::filesystem::path const &path,
      std::function<void(std::string const &, T &output)> &&parser =
          [](std::string const &, T &) {}) noexcept
  : path_(path.string())
  , parser_(std::move(parser))
  {
    file_.open(path);
    if (!file_.is_open())
      LOG(WARNING) << fmt::format("Cannot open {}", path_.c_str());
  }

  std::string source() const override
  {
    return path_;
  }

  bool read(T &data) override
  {
    if (file_.is_open()) {
      file_.clear();
      file_.seekg(0);

      if constexpr (std::is_same_v<T, std::string>) {
        // read the first line into data
        std::getline(file_, data);
      }
      else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        // read the file lines into data, reusing existing data elements as
        // the new data holders when possible

        size_t index = 0;
        while (std::getline(file_, lineData_)) {
          if (data.size() == index)
            data.emplace_back(std::string());

          std::swap(lineData_, data[index++]);
        }
      }
      else {
        // pass the first line to the parser
        std::getline(file_, lineData_);
        parser_(lineData_, data);
      }

      return true;
    }

    return false;
  }

 private:
  std::string const path_;
  std::function<void(std::string const &, T &output)> const parser_;

  std::ifstream file_;
  std::string lineData_;
};
