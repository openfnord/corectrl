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

#include "ihwidtranslator.h"
#include "vendor.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

template<typename...>
class IDataSource;

class HWIDTranslator final : public IHWIDTranslator
{
 public:
  HWIDTranslator(
      std::vector<Vendor> vendors,
      std::unique_ptr<IDataSource<std::vector<char>>> &&dataSource) noexcept;

  // clang-format off
  std::string vendor(std::string const& vendorID) const override;

  std::string device(std::string const& vendorID,
                     std::string const& deviceID) const override;

  std::string subdevice(std::string const& vendorID,
                        std::string const& deviceID,
                        std::string const& subvendorID,
                        std::string const& subdeviceID) const override;
  // clang-format on
 private:
  /// Extracts a name from a line, skipping all information between
  /// the first ocurrence of " (" or " [" and the end of the line.
  /// @returns name
  std::string extractName(std::string const &line, size_t start) const;

  void parseHWIDSFileData(std::vector<char> &data, std::vector<Vendor> &&vendors);

 private:
  std::unordered_map<std::string, std::string> vendors_;
  std::unordered_map<std::string, std::string> devices_;
  std::unordered_map<std::string, std::string> subdevices_;
};
