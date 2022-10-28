// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "zipdatasource.h"

#include "fmt/format.h"
#include <QByteArray>
#include <QIODevice>
#include <QString>
#include <algorithm>
#include <iterator>
#include <quazip.h>
#include <quazipfile.h>
#include <stdexcept>

ZipDataSource::ZipDataSource(std::filesystem::path const &path) noexcept
: path_(path)
{
}

std::string ZipDataSource::source() const
{
  return path_.string();
}

bool ZipDataSource::read(std::string const &internalDataPath,
                         std::vector<char> &data)
{
  if (!internalDataPath.empty()) {
    QuaZip zip(QString::fromStdString(source()));
    if (zip.open(QuaZip::mdUnzip)) {
      if (zip.setCurrentFile(QString::fromStdString(internalDataPath))) {

        QuaZipFile file(&zip);
        if (file.open(QIODevice::ReadOnly)) {

          data.clear();
          auto fileData = file.readAll();
          std::copy(fileData.cbegin(), fileData.cend(), std::back_inserter(data));

          file.close();
          zip.close();
          return true;
        }
      }

      zip.close();
    }
    else {
      throw std::runtime_error(fmt::format("Failed to open file {}", source()));
    }
  }

  return false;
}
