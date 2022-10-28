// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "appfactory.h"

#include "app.h"
#include "common/cryptolayer.h"
#include "config.h"
#include "core/ccpro/ccproparser.h"
#include "core/components/controls/cpucontrolprovider.h"
#include "core/components/controls/gpucontrolprovider.h"
#include "core/components/sensors/cpusensorprovider.h"
#include "core/components/sensors/gpusensorprovider.h"
#include "core/filecache.h"
#include "core/info/hwiddatasource.h"
#include "core/info/hwidtranslator.h"
#include "core/info/infoproviderregistry.h"
#include "core/info/swinfo.h"
#include "core/iprofileparser.h"
#include "core/profile.h"
#include "core/profilefactory.h"
#include "core/profileiconcache.h"
#include "core/profilemanager.h"
#include "core/profilepartprovider.h"
#include "core/profilepartxmlparserprovider.h"
#include "core/profilestorage.h"
#include "core/profileviewfactory.h"
#include "core/profilexmlparserfactory.h"
#include "core/qmlcomponentfactory.h"
#include "core/qmlcomponentregistry.h"
#include "core/session.h"
#include "core/sysexplorer.h"
#include "core/sysmodelfactory.h"
#include "core/sysmodelsyncer.h"
#include "core/uifactory.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "helper/helpercontrol.h"
#include "helper/helpermonitor.h"
#include "helper/helpersysctl.h"
#include <QStandardPaths>
#include <algorithm>
#include <cctype>
#include <exception>
#include <system_error>
#include <utility>

INITIALIZE_EASYLOGGINGPP

namespace fs = std::filesystem;

AppFactory::AppFactory() noexcept
: gpuVendors_{Vendor::AMD}
{
}

std::unique_ptr<App> AppFactory::build() const
{
  try {
    std::string appName(App::Name);
    std::transform(appName.cbegin(), appName.cend(), appName.begin(), ::tolower);

    auto [config, cache] = standardDirectories();
    createAppDirectories(appName, config, cache);

    auto cryptoLayer = std::make_shared<CryptoLayer>();
    auto helperControl = std::make_unique<HelperControl>(cryptoLayer);

    InfoProviderRegistry infoProviderRegistry;
    auto swInfo = std::make_unique<SWInfo>();
    swInfo->initialize(infoProviderRegistry.swInfoProviders());

    SysModelFactory sysModelFactory(
        std::move(swInfo), std::make_unique<SysExplorer>(gpuVendors_),
        std::make_unique<HWIDTranslator>(
            gpuVendors_, std::make_unique<HWIDDataSource>(PCI_IDS_PATH)),
        std::make_unique<CPUControlProvider>(),
        std::make_unique<CPUSensorProvider>(),
        std::make_unique<GPUControlProvider>(),
        std::make_unique<GPUSensorProvider>(), infoProviderRegistry,
        infoProviderRegistry);
    auto sysModel = sysModelFactory.build();

    ProfileFactory profileFactory(std::make_unique<ProfilePartProvider>());
    auto defaultProfile = profileFactory.build(*sysModel);
    ProfileXMLParserFactory parserFactory(
        std::make_unique<ProfilePartXMLParserProvider>());
    auto profileParser = parserFactory.build(*defaultProfile);
    auto profileFileParser = std::make_unique<CCPROParser>();
    auto iconCache = std::make_unique<ProfileIconCache>(
        std::make_unique<FileCache>(cache / appName / "icons"));
    auto profileManager = std::make_unique<ProfileManager>(
        std::move(defaultProfile),
        std::make_unique<ProfileStorage>(
            config / appName / "profiles", std::move(profileParser),
            std::move(profileFileParser), std::move(iconCache)));

    auto sysModelSyncer = std::make_shared<SysModelSyncer>(
        std::move(sysModel), std::make_unique<HelperSysCtl>(cryptoLayer));
    auto session = std::make_unique<Session>(
        sysModelSyncer, std::move(profileManager),
        std::make_unique<ProfileViewFactory>(),
        std::make_unique<HelperMonitor>(cryptoLayer));
    auto uiFactory =
        std::make_unique<UIFactory>(std::make_unique<QMLComponentFactory>(
            std::make_unique<QMLComponentRegistry>()));

    return std::make_unique<App>(std::move(helperControl),
                                 std::move(sysModelSyncer), std::move(session),
                                 std::move(uiFactory));
  }
  catch (std::exception const &e) {
    LOG(WARNING) << "Cannot create main application";
    LOG(WARNING) << e.what();
  }

  return nullptr;
}

std::tuple<std::filesystem::path, std::filesystem::path>
AppFactory::standardDirectories() const
{
  return {QStandardPaths::standardLocations(QStandardPaths::ConfigLocation)
              .first()
              .toStdString(),
          QStandardPaths::standardLocations(QStandardPaths::CacheLocation)
              .first()
              .toStdString()};
}

void AppFactory::createAppDirectories(std::string const &appDirectory,
                                      std::filesystem::path const &config,
                                      std::filesystem::path const &cache) const
{
  std::error_code ec;
  fs::perms dirPerms = fs::perms::owner_all | fs::perms::group_read |
                       fs::perms::group_exec | fs::perms::others_read |
                       fs::perms::others_exec;

  if (!fs::exists(config)) {
    fs::create_directory(config);
    fs::permissions(config, dirPerms, ec);
    if (ec.value() != 0)
      LOG(ERROR) << "Cannot set permissions for " << config;
  }

  if (!fs::is_directory(config))
    throw std::runtime_error(
        fmt::format("{} is not a directory", config.c_str()));

  fs::path appConfigDir = config / appDirectory;
  if (!fs::exists(appConfigDir)) {
    fs::create_directory(appConfigDir);
    fs::permissions(appConfigDir, dirPerms, ec);
    if (ec.value() != 0)
      LOG(ERROR) << "Cannot set permissions for " << appConfigDir;
  }

  if (!fs::is_directory(appConfigDir))
    throw std::runtime_error(
        fmt::format("{} is not a directory", appConfigDir.c_str()));

  if (!fs::exists(cache)) {
    fs::create_directory(cache);
    fs::permissions(cache, dirPerms, ec);
    if (ec.value() != 0)
      LOG(ERROR) << "Cannot set permissions for " << cache;
  }

  if (!fs::is_directory(cache))
    throw std::runtime_error(fmt::format("{} is not a directory", cache.c_str()));

  fs::path cacheApp = cache / appDirectory;
  if (!fs::exists(cacheApp)) {
    fs::create_directory(cacheApp);
    fs::permissions(cacheApp, dirPerms, ec);
    if (ec.value() != 0)
      LOG(ERROR) << "Cannot set permissions for " << cacheApp;
  }

  if (!fs::is_directory(cacheApp))
    throw std::runtime_error(
        fmt::format("{} is not a directory", cacheApp.c_str()));
}
