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
#include <KArchive/KZip>
#include <QByteArray>
#include <QIODevice>
#include <QString>
#include <algorithm>
#include <iterator>
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
    KZip archive(QString::fromStdString(source()));
    if (archive.open(QIODevice::ReadOnly)) {
      auto rootDir = archive.directory();

      auto file = rootDir->file(QString::fromStdString(internalDataPath));
      if (file != nullptr) {
        data.clear();
        auto fileData = file->data();
        std::copy(fileData.cbegin(), fileData.cend(), std::back_inserter(data));
        return true;
      }
    }
    else {
      throw std::runtime_error(fmt::format("Failed to open file {}", source()));
    }
  }

  return false;
}
