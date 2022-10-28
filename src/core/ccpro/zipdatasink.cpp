// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "zipdatasink.h"

#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <QByteArray>
#include <QIODevice>
#include <QString>
#include <exception>
#include <quazip.h>
#include <quazipfile.h>
#include <quazipnewinfo.h>
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
    QuaZip zip(QString::fromStdString(sink()));
    if (zip.open(QuaZip::mdCreate)) {

      for (auto &[dataFilePath, fileData] : data) {
        if (!dataFilePath.empty() && !fileData.empty()) {

          QuaZipFile file(&zip);
          if (!(file.open(QIODevice::WriteOnly,
                          QuaZipNewInfo(QString::fromStdString(dataFilePath))) &&
                file.write(QByteArray::fromRawData(fileData.data(),
                                                   fileData.size())) >= 0)) {

            if (file.isOpen())
              file.close();
            zip.close();

            restorePreWriteFileState();

            throw std::runtime_error(
                fmt::format("Failed to write {} data to file {}",
                            dataFilePath.data(), sink().data()));
          }

          file.close();
        }
      }

      zip.close();
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
