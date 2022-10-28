// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
