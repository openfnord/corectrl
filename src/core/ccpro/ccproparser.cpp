// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "ccproparser.h"

#include "easyloggingpp/easylogging++.h"
#include "zipdatasink.h"
#include "zipdatasource.h"
#include <exception>

std::string CCPROParser::fileExtension() const
{
  return "ccpro";
}

std::optional<std::vector<char>>
CCPROParser::load(std::filesystem::path const &path,
                  std::string const &internalDataName)
{
  ZipDataSource dataSource(path);

  if (!internalDataName.empty()) {
    try {
      std::vector<char> fileData;
      if (dataSource.read(internalDataName, fileData))
        return {fileData};
    }
    catch (std::exception const &e) {
      LOG(ERROR) << e.what();
    }
  }

  return {};
}

bool CCPROParser::save(
    std::filesystem::path const &path,
    std::vector<std::pair<std::string, std::vector<char>>> const &data)
{
  ZipDataSink dataSink(path);

  try {
    return dataSink.write(data);
  }
  catch (std::exception const &e) {
    LOG(ERROR) << e.what();
  }

  return false;
}
