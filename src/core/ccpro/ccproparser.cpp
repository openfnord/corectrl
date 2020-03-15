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
