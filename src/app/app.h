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
#pragma once

#include "config.h"
#include "singleinstance.h"
#include <QObject>
#include <QString>
#include <QVariant>
#include <memory>
#include <string_view>

class IHelperControl;
class ISession;
class ISysModelSyncer;
class IUIFactory;
class QQmlApplicationEngine;
class QQuickWindow;
class Settings;
class SysTray;

class AppInfo final : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString name READ getName CONSTANT)
  Q_PROPERTY(QString version READ getVersion CONSTANT)

 public:
  AppInfo(std::string_view appName, std::string_view appVersion) noexcept
  : QObject()
  , name_(appName.data())
  , version_(appVersion.data())
  {
  }

  QString const &getName() const
  {
    return name_;
  }

  QString const &getVersion() const
  {
    return version_;
  }

 private:
  QString const name_;
  QString const version_;
};

class App final : public QObject
{
  Q_OBJECT

 public:
  static constexpr std::string_view Name{PROJECT_NAME};
  static constexpr std::string_view VersionStr{PROJECT_VERSION};

  App(std::unique_ptr<IHelperControl> &&helperControl,
      std::shared_ptr<ISysModelSyncer> sysSyncer,
      std::unique_ptr<ISession> &&session,
      std::unique_ptr<IUIFactory> &&uiFactory,
      QObject *parent = nullptr) noexcept;

  ~App();

  int exec(int argc, char **argv);

 private slots:
  void exit();
  void showMainWindow();
  void onSettingChanged(QString const &key, QVariant const &value);

 private:
  void buildUI(QQmlApplicationEngine &qmlEngine);

  AppInfo appInfo_;
  SingleInstance singleInstance_;

  std::unique_ptr<IHelperControl> helperControl_;
  std::shared_ptr<ISysModelSyncer> sysSyncer_;
  std::unique_ptr<ISession> session_;
  std::unique_ptr<IUIFactory> uiFactory_;
  std::unique_ptr<Settings> settings_;

  bool noop_{false};
  QQuickWindow *mainWindow_{nullptr};
  std::unique_ptr<SysTray> sysTray_;
};
