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
#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QTimer>

SysTray::SysTray(QObject *parent)
: QObject(parent)
{
  createSysTrayIcon();
}

bool SysTray::isAvailable() const
{
  return sysTray_ != nullptr;
}

bool SysTray::isVisible() const
{
  return sysTray_ != nullptr && sysTray_->isVisible();
}

void SysTray::show()
{
  if (sysTray_ != nullptr)
    sysTray_->show();
}

void SysTray::hide()
{
  if (sysTray_ != nullptr)
    sysTray_->hide();
}

void SysTray::settingChanged(QString const &key, QVariant const &value)
{
  if (sysTray_ != nullptr) {
    if (key == "sysTray")
      sysTray_->setVisible(value.toBool());
  }
}

void SysTray::createSysTrayIcon()
{
  if (QSystemTrayIcon::isSystemTrayAvailable()) {
    sysTray_ = std::make_unique<QSystemTrayIcon>(this);
    sysTray_->setIcon(QIcon::fromTheme(QString(App::Name.data()).toLower()));
    connect(sysTray_.get(), &QSystemTrayIcon::activated, this,
            &SysTray::onTrayIconActivated);

    QMenu *menu = new QMenu();
    QAction *quitAction = new QAction(tr("Quit"), sysTray_.get());
    connect(quitAction, &QAction::triggered, this, &SysTray::quit);
    menu->addAction(quitAction);
    sysTray_->setContextMenu(menu);

    emit available();
  }
  else { // deferred creation
    static int retries{30};
    if (retries > 0) {
      --retries;
      QTimer::singleShot(2000, this, &SysTray::createSysTrayIcon);
    }
  }
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
