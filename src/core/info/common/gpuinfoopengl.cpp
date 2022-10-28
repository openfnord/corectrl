// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "gpuinfoopengl.h"

#include "../infoproviderregistry.h"
#include "core/idatasource.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <QProcess>
#include <QString>
#include <QStringList>
#include <utility>

class GPUInfoOpenGLDataSource : public IDataSource<std::string, int const>
{
 public:
  std::string source() const override
  {
    return "glxinfo";
  }

  bool read(std::string &data, int const &gpuIndex) override
  {
    static constexpr std::string_view libGLErrorStr(
        "libGL error: failed to load driver");

    auto env = QProcessEnvironment::systemEnvironment();
    env.insert("LC_ALL", "C");
    env.insert("DRI_PRIME", QString::number(gpuIndex));

    QProcess cmd;
    cmd.setProcessChannelMode(QProcess::MergedChannels);
    cmd.setProcessEnvironment(env);
    cmd.start(source().c_str(), QStringList("-B"));

    if (cmd.waitForFinished()) {
      auto output = cmd.readAllStandardOutput().toStdString();

      auto libGLErrorPos = output.find(libGLErrorStr);
      if (libGLErrorPos == std::string::npos) {
        data = output;
        return true;
      }

      // extract libGL error
      auto endLinePos = output.find("\n", libGLErrorPos);
      auto libGLError = output.substr(
          libGLErrorPos + libGLErrorStr.length(),
          endLinePos - libGLErrorPos - libGLErrorStr.length());

      LOG(WARNING) << fmt::format(
          "glxinfo command failed for GPU{} with error '{}{}'", gpuIndex,
          libGLErrorStr.data(), libGLError);
    }

    LOG(WARNING) << "glxinfo command failed";
    return false;
  }
};

GPUInfoOpenGL::GPUInfoOpenGL(
    std::unique_ptr<IDataSource<std::string, int const>> &&dataSource) noexcept
: dataSource_(std::move(dataSource))
{
}

std::vector<std::pair<std::string, std::string>>
GPUInfoOpenGL::provideInfo(Vendor, int gpuIndex, IGPUInfo::Path const &,
                           IHWIDTranslator const &)
{
  std::vector<std::pair<std::string, std::string>> info;

  static constexpr std::string_view queryRendererStr("GLX_MESA_query_renderer");
  static constexpr std::string_view coreVerStr("Max core profile version: ");
  static constexpr std::string_view compatVerStr(
      "Max compat profile version: ");

  std::string data;
  if (dataSource_->read(data, gpuIndex)) {
    auto queryRendererPos = data.find(queryRendererStr);
    if (queryRendererPos != std::string::npos) {

      auto coreVer = findItem(data, coreVerStr, queryRendererPos);
      if (!coreVer.empty())
        info.emplace_back(GPUInfoOpenGL::Keys::coreVersion, std::move(coreVer));
      else
        LOG(ERROR) << fmt::format("Cannot find '{}' in glxinfo output",
                                  coreVerStr.data());

      auto compatVer = findItem(data, compatVerStr, queryRendererPos);
      if (!compatVer.empty())
        info.emplace_back(GPUInfoOpenGL::Keys::compatVersion,
                          std::move(compatVer));
      else
        LOG(ERROR) << fmt::format("Cannot find '{}' in glxinfo output",
                                  compatVerStr.data());
    }
    else
      LOG(ERROR) << fmt::format("Cannot find '{}' in glxinfo output",
                                queryRendererStr.data());
  }

  return info;
}

std::vector<std::string>
GPUInfoOpenGL::provideCapabilities(Vendor, int, IGPUInfo::Path const &)
{
  return {};
}

std::string GPUInfoOpenGL::findItem(std::string const &src,
                                    std::string_view itemStr, size_t pos) const
{
  auto itemPos = src.find(itemStr, pos);
  if (itemPos != std::string::npos) {

    auto endLinePos = src.find("\n", itemPos);
    auto item = src.substr(itemPos + itemStr.length(),
                           endLinePos - itemPos - itemStr.length());
    return item;
  }

  return std::string{};
}

bool GPUInfoOpenGL::registered_ = InfoProviderRegistry::add(
    std::make_unique<GPUInfoOpenGL>(std::make_unique<GPUInfoOpenGLDataSource>()));
