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
#include "zipdatasink.h"

#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <KArchive/KZip>
#include <QByteArray>
#include <QIODevice>
#include <QString>
#include <exception>
#include <stdexcept>

namespace fs = std::filesystem;

ZipDataSink::ZipDataSink(std::filesystem::path const &path) noexcept
: path_(path)
{
  backupFile();
}

ZipDataSink::~ZipDataSink()
{
  removeBackupFile();
}

std::string ZipDataSink::sink() const
{
  return path_.string();
}

bool ZipDataSink::write(
    std::vector<std::pair<std::string, std::vector<char>>> const &data)
{
  if (!data.empty()) {
    KZip archive(QString::fromStdString(sink()));
    if (archive.open(QIODevice::WriteOnly)) {

      for (auto &[dataFilePath, fileData] : data) {
        if (!dataFilePath.empty() && !fileData.empty()) {
          if (!archive.writeFile(
                  QString::fromStdString(dataFilePath),
                  QByteArray::fromRawData(fileData.data(), fileData.size()))) {
            restorePreWriteFileState();
            throw std::runtime_error(
                fmt::format("Failed to write {} data to file {}",
                            dataFilePath.data(), sink().data()));
          }
        }
      }

      return true;
    }
    else {
      throw std::runtime_error(
          fmt::format("Failed to open file {}", sink().data()));
    }
  }

  return false;
}

void ZipDataSink::backupFile() const
{
  try {
    if (fs::exists(path_) && fs::is_regular_file(path_))
      fs::copy_file(path_, sink() + ".bak", fs::copy_options::overwrite_existing);
  }
  catch (std::exception const &e) {
    LOG(ERROR) << e.what();
  }
}

void ZipDataSink::removeBackupFile() const
{
  try {
    fs::remove(sink() + ".bak");
  }
  catch (std::exception const &e) {
    LOG(ERROR) << e.what();
  }
}

void ZipDataSink::restorePreWriteFileState() const
{
  // remove the current file
  try {
    fs::remove(path_.string());
  }
  catch (std::exception const &e) {
    LOG(ERROR) << e.what();
  }

  // restore the backup file
  try {
    if (fs::exists(sink() + ".bak") && fs::is_regular_file(sink() + ".bak"))
      fs::copy_file(sink() + ".bak", path_, fs::copy_options::overwrite_existing);
  }
  catch (std::exception const &e) {
    LOG(ERROR) << e.what();
  }
}
