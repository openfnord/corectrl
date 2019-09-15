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
#include <QMenu>

SysTray::SysTray(QObject *parent)
: QObject(parent)
{
  sysTray_ = new QSystemTrayIcon(this);
  sysTray_->setIcon(QIcon::fromTheme(QString(App::Name.data()).toLower()));
  connect(sysTray_, &QSystemTrayIcon::activated, this,
          &SysTray::onTrayIconActivated);

  QMenu *menu = new QMenu();
  QAction *quitAction = new QAction(tr("Quit"), sysTray_);
  connect(quitAction, &QAction::triggered, this, &SysTray::quit);
  menu->addAction(quitAction);

  sysTray_->setContextMenu(menu);
}

bool SysTray::isAvailable() const
{
  return sysTray_->isSystemTrayAvailable();
}

bool SysTray::isVisible() const
{
  return sysTray_->isVisible();
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
