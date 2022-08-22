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

#include "core/iprofile.h"
#include <QMenu>
#include <QObject>
#include <QString>
#include <QSystemTrayIcon>
#include <QVariant>
#include <memory>
#include <optional>
#include <string>

class IProfileManager;
class ISession;

class SysTray : public QObject
{
  Q_OBJECT
 public:
  explicit SysTray(ISession *session, QObject *parent = nullptr);

  Q_INVOKABLE bool isAvailable() const;
  Q_INVOKABLE bool isVisible() const;

 signals:
  void activated();
  void quit();

 public slots:
  void show();
  void hide();
  void settingChanged(QString const &key, QVariant const &value);

 private slots:
  void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

 private:
  void profileAdded(std::string const &profileName);
  void profileRemoved(std::string const &profileName);
  void profileInfoChanged(IProfile::Info const &oldInfo,
                          IProfile::Info const &newInfo);
  void manualProfileToggled(std::string const &profileName, bool active);

  QSystemTrayIcon *createSystemTrayIcon();
  QMenu *menu();
  QAction *createManualProfileAction(QMenu *menu, std::string const &profileName);
  std::optional<QAction *> findManualProfileAction(std::string const &profileName);
  QAction *findNextManualProfileActionPosition(std::string const &profileName);
  void addManualProfilesTo(QMenu *menu);
  void onManualProfileMenuTriggered(QString const &profile);

  ISession *session_;
  IProfileManager *profileManager_;
  QSystemTrayIcon *sysTray_{nullptr};
  QMenu menu_;
  QMenu *manualProfileMenu_{nullptr};

  class ProfileManagerObserver;
  std::shared_ptr<ProfileManagerObserver> profileManagerObserver_;

  class ManualProfileObserver;
  std::shared_ptr<ManualProfileObserver> manualProfileObserver_;
};
