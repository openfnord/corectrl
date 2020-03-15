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
#include "swinfomesa.h"

#include "../infoproviderregistry.h"
#include "core/idatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <QProcess>
#include <string_view>
#include <utility>

class SWInfoMesaDataSource : public IDataSource<std::string>
{
 public:
  std::string source() const override
  {
    return "glxinfo -B";
  }

  bool read(std::string &data) override
  {
    auto env = QProcessEnvironment::systemEnvironment();
    env.insert("LC_ALL", "C");

    QProcess cmd;
    cmd.setProcessChannelMode(QProcess::MergedChannels);
    cmd.setProcessEnvironment(env);
    cmd.start(source().c_str());

    if (cmd.waitForFinished()) {
      data = cmd.readAllStandardOutput().toStdString();
      return true;
    }

    LOG(WARNING) << "glxinfo command failed";
    return false;
  }
};

SWInfoMesa::SWInfoMesa(std::unique_ptr<IDataSource<std::string>> &&dataSource) noexcept
: dataSource_(std::move(dataSource))
{
}

std::vector<std::pair<std::string, std::string>> SWInfoMesa::provideInfo()
{
  std::vector<std::pair<std::string, std::string>> info;

  static constexpr std::string_view queryRendererStr("GLX_MESA_query_renderer");
  static constexpr std::string_view versionStr("Version: ");

  std::string data;
  if (dataSource_->read(data)) {
    auto queryRendererPos = data.find(queryRendererStr);
    if (queryRendererPos != std::string::npos) {

      auto versionPos = data.find(versionStr, queryRendererPos);
      if (versionPos != std::string::npos) {

        auto endLinePos = data.find("\n", versionPos);
        auto version = data.substr(versionPos + versionStr.length(),
                                   endLinePos - versionPos - versionStr.length());

        info.emplace_back(ISWInfo::Keys::mesaVersion, std::move(version));
      }
      else
        LOG(ERROR) << fmt::format("Cannot find '{}' in glxinfo output",
                                  versionStr.data());
    }
    else
      LOG(ERROR) << fmt::format("Cannot find '{}' in glxinfo output",
                                queryRendererStr.data());
  }

  return info;
}

bool const SWInfoMesa::registered_ = InfoProviderRegistry::add(
    std::make_unique<SWInfoMesa>(std::make_unique<SWInfoMesaDataSource>()));
