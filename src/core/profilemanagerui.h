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

#include <QObject>
#include <QSet>
#include <QString>
#include <QUrl>
#include <QVariantList>
#include <memory>
#include <string>
#include <string_view>

class IProfileManager;
class ISysModelUI;
class IFileCache;

class ProfileManagerUI : public QObject
{
  Q_OBJECT

 public:
  static constexpr std::string_view QMLComponentID{"PROFILE_MANAGER"};

  explicit ProfileManagerUI(QObject *parent = nullptr) noexcept;

  void init(IProfileManager *profileManager, ISysModelUI *sysModelUI);

  Q_INVOKABLE QString defaultIcon() const;
  Q_INVOKABLE bool isProfileNameInUse(QString const &profileName);
  Q_INVOKABLE bool isExecutableNameInUse(QString const &executableName);
  Q_INVOKABLE bool isProfileUnsaved(QString const &profileName);
  Q_INVOKABLE bool isProfileActive(QString const &profileName);

  Q_INVOKABLE void add(QString const &name, QString const &exe,
                       QString const &icon, QString const &base);
  Q_INVOKABLE void remove(QString const &name);
  Q_INVOKABLE void updateInfo(QString const &oldName, QString const &newName,
                              QString const &exe, QString const &icon);
  Q_INVOKABLE void activate(QString const &name, bool active);

  Q_INVOKABLE void loadSettings(QString const &name);
  Q_INVOKABLE bool loadSettings(QString const &name, QUrl const &path);
  Q_INVOKABLE void applySettings(QString const &name);
  Q_INVOKABLE void resetSettings(QString const &name);
  Q_INVOKABLE void restoreSettings(QString const &name);
  Q_INVOKABLE void saveSettings(QString const &name);
  Q_INVOKABLE bool exportProfile(QString const &name, QUrl const &path);

 signals:
  void initProfiles(QVariantList const &profiles);
  void profileAdded(QString const &name, QString const &exe,
                    QString const &icon, bool isActive);
  void profileRemoved(QString const &name);
  void profileChanged(QString const &name);
  void profileActiveChanged(QString const &name, bool active);
  void profileSaved(QString const &name);
  void profileInfoChanged(QString const &oldName, QString const &newName,
                          QString const &exe, QString const &icon,
                          bool isActive);

 private:
  std::string cleanIconFilePath(QString iconPath) const;
  QString toQMLIconPath(std::string const &iconPath) const;

  void addProfileComponet(std::string const &profileName);
  void addProfileUsedNames(std::string const &profileName);
  void removeProfileUsedNames(std::string const &profileName);

  IProfileManager *profileManager_{nullptr};
  ISysModelUI *sysModelUI_{nullptr};

  class ProfileManagerObserver;
  std::shared_ptr<ProfileManagerObserver> profileManagerObserver_;

  QSet<QString> usedProfileNames_;
  QSet<QString> usedExecutableNames_;

  static bool const registered_;
  static char const *const trStrings[];
};
