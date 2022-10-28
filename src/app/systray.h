// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/iprofile.h"
#include <QAction>
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
  void showMainWindowToggled(bool visible);

 public slots:
  void show();
  void hide();
  void settingChanged(QString const &key, QVariant const &value);
  void onMainWindowVisibleChanged(bool isVisible);

 private slots:
  void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
  void onShowMainWindowActionTriggered();

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
  QAction *showMainWindowAction_{nullptr};
  bool showMainWindow_;

  class ProfileManagerObserver;
  std::shared_ptr<ProfileManagerObserver> profileManagerObserver_;

  class ManualProfileObserver;
  std::shared_ptr<ManualProfileObserver> manualProfileObserver_;
};
