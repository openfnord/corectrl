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
