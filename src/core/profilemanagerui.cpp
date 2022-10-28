// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "profilemanagerui.h"

#include "iprofile.h"
#include "iprofilemanager.h"
#include "isession.h"
#include "isysmodelui.h"
#include "qmlcomponentregistry.h"
#include <QQmlApplicationEngine>
#include <QtGlobal>
#include <QtQml>
#include <algorithm>
#include <utility>

char const *const ProfileManagerUI::trStrings[] = {
    QT_TRANSLATE_NOOP("ProfileManagerUI", "_global_"),
};

class ProfileManagerUI::ProfileManagerObserver : public IProfileManager::Observer
{
 public:
  ProfileManagerObserver(ProfileManagerUI &outer) noexcept
  : outer_(outer)
  {
  }

  void profileAdded(std::string const &profileName) override;
  void profileRemoved(std::string const &profileName) override;
  void profileChanged(std::string const &profileName) override;
  void profileActiveChanged(std::string const &profileName, bool active) override;
  void profileSaved(std::string const &profileName) override;
  void profileInfoChanged(IProfile::Info const &oldInfo,
                          IProfile::Info const &newInfo) override;

 private:
  ProfileManagerUI &outer_;
};

void ProfileManagerUI::ProfileManagerObserver::profileAdded(
    std::string const &profileName)
{
  outer_.addProfileComponet(profileName);
}

void ProfileManagerUI::ProfileManagerObserver::profileRemoved(
    std::string const &profileName)
{
  emit outer_.profileRemoved(QString::fromStdString(profileName));
}

void ProfileManagerUI::ProfileManagerObserver::profileChanged(
    std::string const &profileName)
{
  emit outer_.profileChanged(QString::fromStdString(profileName));
}

void ProfileManagerUI::ProfileManagerObserver::profileActiveChanged(
    std::string const &profileName, bool active)
{
  emit outer_.profileActiveChanged(QString::fromStdString(profileName), active);
}

void ProfileManagerUI::ProfileManagerObserver::profileSaved(
    std::string const &profileName)
{
  emit outer_.profileSaved(QString::fromStdString(profileName));
}

void ProfileManagerUI::ProfileManagerObserver::profileInfoChanged(
    IProfile::Info const &oldInfo, IProfile::Info const &newInfo)
{
  outer_.addProfileUsedNames(newInfo.name);
  auto profile = outer_.profileManager_->profile(newInfo.name);
  emit outer_.profileInfoChanged(
      QString::fromStdString(oldInfo.name), QString::fromStdString(newInfo.name),
      newInfo.exe != IProfile::Info::ManualID
          ? QString::fromStdString(newInfo.exe)
          : QLatin1String(""),
      outer_.toQMLIconPath(newInfo.iconURL), newInfo.hasCustomIcon(),
      profile->get().active());
}

class ProfileManagerUI::ManualProfileObserver
: public ISession::ManualProfileObserver
{
 public:
  ManualProfileObserver(ProfileManagerUI &outer) noexcept
  : outer_(outer)
  {
  }

  void toggled(std::string const &profileName, bool active) override;

 private:
  ProfileManagerUI &outer_;
};

void ProfileManagerUI::ManualProfileObserver::toggled(
    const std::string &profileName, bool active)
{
  emit outer_.manualProfileToggled(QString::fromStdString(profileName), active);
}

ProfileManagerUI::ProfileManagerUI(QObject *parent) noexcept
: QObject(parent)
, profileManagerObserver_(
      std::make_shared<ProfileManagerUI::ProfileManagerObserver>(*this))
, manualProfileObserver_(
      std::make_shared<ProfileManagerUI::ManualProfileObserver>(*this))
{
  usedExecutableNames_.insert(
      QString::fromLatin1(IProfile::Info::ManualID.data()));
}

void ProfileManagerUI::init(ISession *session, ISysModelUI *sysModelUI)
{
  sysModelUI_ = sysModelUI;
  session_ = session;
  session_->addManualProfileObserver(manualProfileObserver_);
  profileManager_ = &session_->profileManager();
  profileManager_->addObserver(profileManagerObserver_);

  // create profile components
  auto profileNames = profileManager_->profiles();

  // short profile names
  std::sort(profileNames.begin(), profileNames.end());

  // the global profile must be the first element
  auto globalIt = std::find_if(
      profileNames.begin(), profileNames.end(),
      [](auto &profile) { return profile == IProfile::Info::GlobalID; });
  if (globalIt != profileNames.cend())
    std::rotate(profileNames.begin(), globalIt, globalIt + 1);

  QVariantList list;
  for (auto &profileName : profileNames) {
    addProfileUsedNames(profileName);

    auto profile = profileManager_->profile(profileName);
    auto &info = profile->get().info();

    list.append(QString::fromStdString(info.name));
    list.append(info.exe != IProfile::Info::ManualID
                    ? QString::fromStdString(info.exe)
                    : QLatin1String(""));
    list.append(toQMLIconPath(info.iconURL));
    list.append(profile->get().active());
    list.append(info.hasCustomIcon());
  }

  emit initProfiles(list);
}

QString ProfileManagerUI::defaultIcon() const
{
  return toQMLIconPath(IProfile::Info::DefaultIconURL.data());
}

bool ProfileManagerUI::isProfileNameInUse(QString const &profileName)
{
  return usedProfileNames_.contains(profileName);
}

bool ProfileManagerUI::isExecutableNameInUse(QString const &executableName)
{
  return usedExecutableNames_.contains(executableName);
}

bool ProfileManagerUI::isProfileUnsaved(QString const &profileName)
{
  return profileManager_->unsaved(profileName.toStdString());
}

bool ProfileManagerUI::isProfileActive(QString const &profileName)
{
  auto profile = profileManager_->profile(profileName.toStdString());
  if (profile.has_value())
    return profile->get().active();

  return false;
}

void ProfileManagerUI::add(QString const &name, QString const &exe,
                           QString const &icon, QString const &base)
{
  IProfile::Info info(name.toStdString(),
                      exe.length() > 0 ? exe.toStdString()
                                       : std::string(IProfile::Info::ManualID));
  info.iconURL = cleanIconFilePath(icon);

  if (base == "defaultProfile")
    profileManager_->add(std::move(info));
  else
    profileManager_->clone(std::move(info), base.toStdString());
}

void ProfileManagerUI::remove(QString const &name)
{
  auto profileName = name.toStdString();
  removeProfileUsedNames(profileName);
  profileManager_->remove(profileName);
}

void ProfileManagerUI::updateInfo(QString const &oldName, QString const &newName,
                                  QString const &exe, QString const &icon)
{
  auto profileName = oldName.toStdString();
  removeProfileUsedNames(profileName);
  IProfile::Info info(newName.toStdString(),
                      exe.length() > 0 ? exe.toStdString()
                                       : std::string(IProfile::Info::ManualID),
                      cleanIconFilePath(icon));
  profileManager_->update(profileName, info);
}

void ProfileManagerUI::activate(QString const &name, bool active)
{
  profileManager_->activate(name.toStdString(), active);
}

void ProfileManagerUI::toggleManualProfile(QString const &name)
{
  session_->toggleManualProfile(name.toStdString());
}

void ProfileManagerUI::loadSettings(QString const &name)
{
  auto profile = profileManager_->profile(name.toStdString());
  if (profile.has_value())
    profile->get().exportWith(*sysModelUI_);
}

bool ProfileManagerUI::loadSettings(QString const &name, QUrl const &path)
{
  if (profileManager_->loadFrom(
          name.toStdString(), path.toString(QUrl::RemoveScheme).toStdString())) {
    loadSettings(name);

    return true;
  }

  return false;
}

void ProfileManagerUI::applySettings(QString const &name)
{
  profileManager_->update(name.toStdString(), *sysModelUI_);
}

void ProfileManagerUI::resetSettings(QString const &name)
{
  profileManager_->reset(name.toStdString());
  loadSettings(name);
}

void ProfileManagerUI::restoreSettings(QString const &name)
{
  profileManager_->restore(name.toStdString());
  loadSettings(name);
}

void ProfileManagerUI::saveSettings(QString const &name)
{
  profileManager_->save(name.toStdString());
}

bool ProfileManagerUI::exportProfile(QString const &name, QUrl const &path)
{
  return profileManager_->exportTo(
      name.toStdString(), path.toString(QUrl::RemoveScheme).toStdString());
}

std::string ProfileManagerUI::cleanIconFilePath(QString iconPath) const
{
  if (iconPath.startsWith("file://"))
    iconPath.remove("file://");
  else if (iconPath.startsWith("qrc:"))
    iconPath.remove(0, 3);

  return iconPath.toStdString();
}

QString ProfileManagerUI::toQMLIconPath(std::string const &iconPath) const
{
  auto qtIconPath = QString::fromStdString(iconPath);
  if (qtIconPath.startsWith(":/"))
    qtIconPath.prepend("qrc");
  else
    qtIconPath.prepend("file://");

  return qtIconPath;
}

void ProfileManagerUI::addProfileComponet(std::string const &profileName)
{
  addProfileUsedNames(profileName);

  auto profile = profileManager_->profile(profileName);
  auto &info = profile->get().info();
  emit profileAdded(QString::fromStdString(info.name),
                    info.exe != IProfile::Info::ManualID
                        ? QString::fromStdString(info.exe)
                        : QLatin1String(""),
                    toQMLIconPath(info.iconURL), info.hasCustomIcon(),
                    profile->get().active());
}

void ProfileManagerUI::addProfileUsedNames(std::string const &profileName)
{
  auto profile = profileManager_->profile(profileName);
  auto &profileInfo = profile->get().info();

  auto name = QString::fromStdString(profileName);
  usedProfileNames_.insert(name);

  if (profileInfo.exe != IProfile::Info::ManualID) {
    auto exe = QString::fromStdString(profileInfo.exe);
    usedExecutableNames_.insert(exe);
  }
}

void ProfileManagerUI::removeProfileUsedNames(std::string const &profileName)
{
  auto profile = profileManager_->profile(profileName);
  auto &profileInfo = profile->get().info();

  auto name = QString::fromStdString(profileName);
  usedProfileNames_.remove(name);

  if (profileInfo.exe != IProfile::Info::ManualID) {
    auto exe = QString::fromStdString(profileInfo.exe);
    usedExecutableNames_.remove(exe);
  }
}

bool const ProfileManagerUI::registered_ =
    QMLComponentRegistry::addQMLTypeRegisterer([]() {
      qmlRegisterType<ProfileManagerUI>("CoreCtrl.UIComponents", 1, 0,
                                        ProfileManagerUI::QMLComponentID.data());
    });
