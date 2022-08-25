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
#include "systray.h"

#include "app.h"
#include "core/iprofile.h"
#include "core/iprofilemanager.h"
#include "core/isession.h"
#include <QIcon>
#include <QTimer>
#include <algorithm>

class SysTray::ProfileManagerObserver : public IProfileManager::Observer
{
 public:
  ProfileManagerObserver(SysTray &outer) noexcept
  : outer_(outer)
  {
  }

  void profileAdded(std::string const &profileName) override;
  void profileRemoved(std::string const &profileName) override;
  void profileInfoChanged(IProfile::Info const &oldInfo,
                          IProfile::Info const &newInfo) override;

  void profileChanged(std::string const &) override
  {
  }
  void profileActiveChanged(std::string const &, bool) override
  {
  }
  void profileSaved(std::string const &) override
  {
  }

 private:
  SysTray &outer_;
};

void SysTray::ProfileManagerObserver::profileAdded(std::string const &profileName)
{
  outer_.profileAdded(profileName);
}

void SysTray::ProfileManagerObserver::profileRemoved(std::string const &profileName)
{
  outer_.profileRemoved(profileName);
}

void SysTray::ProfileManagerObserver::profileInfoChanged(
    IProfile::Info const &oldInfo, IProfile::Info const &newInfo)
{
  outer_.profileInfoChanged(oldInfo, newInfo);
}

class SysTray::ManualProfileObserver : public ISession::ManualProfileObserver
{
 public:
  ManualProfileObserver(SysTray &outer) noexcept
  : outer_(outer)
  {
  }

  void toggled(std::string const &profileName, bool active) override;

 private:
  SysTray &outer_;
};

void SysTray::ManualProfileObserver::toggled(const std::string &profileName,
                                             bool active)
{
  outer_.manualProfileToggled(profileName, active);
}

SysTray::SysTray(ISession *session, QObject *parent)
: QObject(parent)
, session_(session)
, profileManager_(&session->profileManager())
, menu_()
, profileManagerObserver_(
      std::make_shared<SysTray::ProfileManagerObserver>(*this))
, manualProfileObserver_(std::make_shared<SysTray::ManualProfileObserver>(*this))
{
  session_->addManualProfileObserver(manualProfileObserver_);
  profileManager_->addObserver(profileManagerObserver_);

  sysTray_ = createSystemTrayIcon();
}

bool SysTray::isAvailable() const
{
  return QSystemTrayIcon::isSystemTrayAvailable();
}

bool SysTray::isVisible() const
{
  return isAvailable() && sysTray_->isVisible();
}

void SysTray::show()
{
  sysTray_->show();
}

void SysTray::hide()
{
  sysTray_->hide();
}

void SysTray::settingChanged(QString const &key, QVariant const &value)
{
  if (key == "sysTray")
    sysTray_->setVisible(value.toBool());
}

void SysTray::onMainWindowVisibleChanged(bool isVisible)
{
  showMainWindow_ = isVisible;
  showMainWindowAction_->setText(isVisible ? tr("Hide") : tr("Show"));
}

void SysTray::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
  switch (reason) {
    case QSystemTrayIcon::Trigger:
      emit activated();
      break;

    default:
      break;
  }
}

void SysTray::onShowMainWindowActionTriggered()
{
  emit showMainWindowToggled(!showMainWindow_);
}

QSystemTrayIcon *SysTray::createSystemTrayIcon()
{
  auto sysTray = new QSystemTrayIcon(this);

  sysTray->setIcon(QIcon::fromTheme(QString(App::Name.data()).toLower()));
  sysTray->setContextMenu(menu());

  connect(sysTray, &QSystemTrayIcon::activated, this,
          &SysTray::onTrayIconActivated);

  return sysTray;
}

QMenu *SysTray::menu()
{
  if (menu_.isEmpty()) {
    showMainWindowAction_ = new QAction(&menu_);
    onMainWindowVisibleChanged(false); // initialize label and state
    connect(showMainWindowAction_, &QAction::triggered, this,
            &SysTray::onShowMainWindowActionTriggered);
    menu_.addAction(showMainWindowAction_);
    menu_.addSeparator();

    manualProfileMenu_ = menu_.addMenu(tr("Manual profiles"));
    addManualProfilesTo(manualProfileMenu_);
    menu_.addSeparator();

    QAction *quitAction = new QAction(tr("Quit"), &menu_);
    connect(quitAction, &QAction::triggered, this, &SysTray::quit);
    menu_.addAction(quitAction);
  }

  return &menu_;
}

QAction *SysTray::createManualProfileAction(QMenu *menu,
                                            std::string const &profileName)
{

  QAction *action = new QAction(QString::fromStdString(profileName), menu);
  action->setCheckable(true);

  connect(action, &QAction::triggered, this,
          [=]() { onManualProfileMenuTriggered(action->text()); });

  return action;
}

std::optional<QAction *>
SysTray::findManualProfileAction(std::string const &profileName)
{
  auto name = QString::fromStdString(profileName);
  auto actions = manualProfileMenu_->actions();

  auto actionIt = std::find_if(
      actions.begin(), actions.end(),
      [&](QAction *action) { return action->text() == name; });

  return actionIt != actions.end() ? std::make_optional(*actionIt)
                                   : std::nullopt;
}

QAction *
SysTray::findNextManualProfileActionPosition(std::string const &profileName)
{
  auto name = QString::fromStdString(profileName);
  auto actions = manualProfileMenu_->actions();

  auto actionIt = std::find_if(
      actions.begin(), actions.end(),
      [&](QAction *action) { return action->text() > name; });

  return actionIt != actions.end() ? *actionIt : nullptr;
}

void SysTray::addManualProfilesTo(QMenu *menu)
{
  auto profiles = profileManager_->profiles();
  std::sort(profiles.begin(), profiles.end());

  for (auto const &profileName : profiles) {

    auto const &profile = profileManager_->profile(profileName);
    if (profile->get().info().exe == IProfile::Info::ManualID) {

      auto action = createManualProfileAction(menu, profileName);
      menu->addAction(action);
    }
  }

  menu->setDisabled(menu->isEmpty());
}

void SysTray::onManualProfileMenuTriggered(QString const &profile)
{
  session_->toggleManualProfile(profile.toStdString());
}

void SysTray::profileAdded(std::string const &profileName)
{
  auto profile = profileManager_->profile(profileName);
  if (profile.has_value() &&
      profile->get().info().exe == IProfile::Info::ManualID) {

    auto action = createManualProfileAction(manualProfileMenu_, profileName);
    auto beforeAction = findNextManualProfileActionPosition(profileName);
    manualProfileMenu_->insertAction(beforeAction, action);
    manualProfileMenu_->setDisabled(false);
  }
}

void SysTray::profileRemoved(std::string const &profileName)
{
  auto action = findManualProfileAction(profileName);
  if (action.has_value()) {
    manualProfileMenu_->removeAction(*action);
    delete *action;

    manualProfileMenu_->setDisabled(manualProfileMenu_->isEmpty());
  }
}

void SysTray::profileInfoChanged(IProfile::Info const &oldInfo,
                                 IProfile::Info const &newInfo)
{
  if (oldInfo.exe == IProfile::Info::ManualID ||
      newInfo.exe == IProfile::Info::ManualID) {

    // automatic profile converted to manual profile
    if (oldInfo.exe != IProfile::Info::ManualID &&
        newInfo.exe == IProfile::Info::ManualID) {
      auto action = createManualProfileAction(manualProfileMenu_, newInfo.name);
      auto beforeAction = findNextManualProfileActionPosition(newInfo.name);
      manualProfileMenu_->insertAction(beforeAction, action);
    }
    // manual profile converted to automatic profile
    else if (oldInfo.exe == IProfile::Info::ManualID &&
             newInfo.exe != IProfile::Info::ManualID) {
      profileRemoved(oldInfo.name);
    }
    // manual profile name has changed
    else if (oldInfo.name != newInfo.name) {
      auto action = findManualProfileAction(oldInfo.name);
      if (action.has_value()) {

        // remove old action
        auto isChecked = (*action)->isChecked();
        manualProfileMenu_->removeAction(*action);
        delete *action;

        // insert a new action
        auto action = createManualProfileAction(manualProfileMenu_, newInfo.name);
        action->setChecked(isChecked);
        auto beforeAction = findNextManualProfileActionPosition(newInfo.name);
        manualProfileMenu_->insertAction(beforeAction, action);
      }
    }

    manualProfileMenu_->setDisabled(manualProfileMenu_->isEmpty());
  }
}

void SysTray::manualProfileToggled(std::string const &profileName, bool active)
{
  auto action = findManualProfileAction(profileName);
  if (action.has_value())
    (*action)->setChecked(active);
}
