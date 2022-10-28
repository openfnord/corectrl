// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "swinfomesa.h"

#include "../infoproviderregistry.h"
#include "core/idatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <QProcess>
#include <QStringList>
#include <string_view>
#include <utility>

class SWInfoMesaDataSource : public IDataSource<std::string>
{
 public:
  std::string source() const override
  {
    return "glxinfo";
  }

  bool read(std::string &data) override
  {
    auto env = QProcessEnvironment::systemEnvironment();
    env.insert("LC_ALL", "C");

    QProcess cmd;
    cmd.setProcessChannelMode(QProcess::MergedChannels);
    cmd.setProcessEnvironment(env);
    cmd.start(source().c_str(), QStringList("-B"));

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
