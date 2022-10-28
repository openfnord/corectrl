// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "hwidtranslator.h"

#include "core/idatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <algorithm>
#include <cctype>
#include <exception>
#include <sstream>
#include <utility>

HWIDTranslator::HWIDTranslator(
    std::vector<Vendor> vendors,
    std::unique_ptr<IDataSource<std::vector<char>>> &&dataSource) noexcept
{
  std::vector<char> data;
  try {
    if (dataSource->read(data))
      parseHWIDSFileData(data, std::move(vendors));
  }
  catch (std::exception const &e) {
    LOG(ERROR) << e.what();
  }
}

std::string HWIDTranslator::vendor(std::string const &vendorID) const
{
  if (!vendors_.empty()) {
    std::string key(vendorID);
    std::transform(key.cbegin(), key.cend(), key.begin(), ::tolower);

    auto const vendorIt = vendors_.find(key);
    if (vendorIt != vendors_.cend())
      return vendorIt->second;
  }

  return std::string{};
}

std::string HWIDTranslator::device(std::string const &vendorID,
                                   std::string const &deviceID) const
{
  if (!devices_.empty()) {
    std::string key;
    key.reserve(vendorID.length() + deviceID.length());

    key.append(vendorID).append(deviceID);
    std::transform(key.cbegin(), key.cend(), key.begin(), ::tolower);

    auto const deviceIt = devices_.find(key);
    if (deviceIt != devices_.cend())
      return deviceIt->second;
  }

  return std::string{};
}

std::string HWIDTranslator::subdevice(std::string const &vendorID,
                                      std::string const &deviceID,
                                      std::string const &subvendorID,
                                      std::string const &subdeviceID) const
{
  if (!subdevices_.empty()) {
    std::string key;
    key.reserve(vendorID.length() + deviceID.length() + subvendorID.length() +
                subdeviceID.length());
    key.append(vendorID).append(deviceID).append(subvendorID).append(subdeviceID);
    std::transform(key.cbegin(), key.cend(), key.begin(), ::tolower);

    auto const subdeviceIt = subdevices_.find(key);
    if (subdeviceIt != subdevices_.cend())
      return subdeviceIt->second;
  }

  return std::string{};
}

std::string HWIDTranslator::extractName(std::string const &line,
                                        size_t start) const
{
  auto nameStartPos = line.find_first_not_of(' ', start);
  if (nameStartPos != std::string::npos) {
    auto bracketPos = line.find(" [", nameStartPos);
    auto parenPos = line.find(" (", nameStartPos);
    auto nameEndPos = std::min({bracketPos, parenPos, std::string::npos});
    return line.substr(nameStartPos, nameEndPos - nameStartPos);
  }

  return std::string{};
}

void HWIDTranslator::parseHWIDSFileData(std::vector<char> &data,
                                        std::vector<Vendor> &&vendors)
{
  std::istringstream dataStream;
  dataStream.rdbuf()->pubsetbuf(data.data(), static_cast<long>(data.size()));

  std::string line;
  line.reserve(200);
  std::string vendorID;
  vendorID.reserve(4);
  std::string deviceID;
  deviceID.reserve(4);
  std::string subvendorID;
  subvendorID.reserve(4);
  std::string subdeviceID;
  subdeviceID.reserve(4);

  std::string devicesKey;
  devicesKey.reserve(8);
  std::string subdevicesKey;
  subdevicesKey.reserve(16);

  std::vector<char> skipLineChars{'#', '\0', 'C'};
  bool skipVendor = true;
  size_t parsedVendorCount = 0;
  while (parsedVendorCount < vendors.size() && std::getline(dataStream, line)) {
    auto const firstChar = line[0];

    // if the line starts with one of skipLineChars, ignore the line
    if (std::find(skipLineChars.cbegin(), skipLineChars.cend(), firstChar) !=
        skipLineChars.cend()) {
      if (firstChar == 'C')
        skipVendor = true;
    }
    // vendor
    else if (firstChar != '\t') {
      if (!skipVendor) // vendor parsed!
        ++parsedVendorCount;

      auto spacePos = line.find(' ');
      vendorID = line.substr(0, spacePos);
      int vendorIDNum = -1;

      try {
        vendorIDNum = std::stoi(vendorID, nullptr, 16);

        // only parse vendors from vendorIDs
        if (std::find(vendors.cbegin(), vendors.cend(),
                      static_cast<Vendor>(vendorIDNum)) == vendors.cend()) {
          skipVendor = true;
        }
        else {
          vendors_.insert({vendorID, extractName(line, spacePos)});
          skipVendor = false;
        }
      }
      catch (std::exception const &e) {
        LOG(ERROR) << fmt::format(
            "Cannot convert vendor id {} from hwdata file.\nError: {}",
            vendorID.c_str(), e.what());
        skipVendor = true;
      }
    }
    // device or subvendor subdevice
    else {
      if (!skipVendor) {
        auto secondChar = line[1];

        // device
        if (secondChar != '\t') {
          auto spacePos = line.find(' ', 1);
          deviceID = line.substr(1, spacePos - 1);

          devicesKey.clear();
          devicesKey.append(vendorID).append(deviceID);

          devices_.insert({devicesKey, extractName(line, spacePos)});
        }
        // subvendor subdevice
        else if (line.length() > 2) {
          auto subvendorSpacePos = line.find(' ', 2);
          subvendorID = line.substr(2, subvendorSpacePos - 2);

          auto subdeviceStartPos = line.find_first_not_of(' ', subvendorSpacePos);
          auto subdeviceEndPos = line.find(' ', subdeviceStartPos);
          subdeviceID = line.substr(subdeviceStartPos,
                                    subdeviceEndPos - subdeviceStartPos);

          subdevicesKey.clear();
          subdevicesKey.append(vendorID)
              .append(deviceID)
              .append(subvendorID)
              .append(subdeviceID);

          subdevices_.insert({subdevicesKey, extractName(line, subdeviceEndPos)});
        }
      }
    }
  }
}
