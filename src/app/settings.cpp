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
#include "settings.h"

Settings::Settings(QString const &appName) noexcept
: QSettings(QSettings::IniFormat, QSettings::UserScope, appName, appName)
{
}

void Settings::setValue(QString const &key, QVariant const &value)
{
  QSettings::setValue(key, value);
  emit settingChanged(key, value);
}

QVariant Settings::getValue(QString const &key, QVariant const &defaultValue) const
{
  auto value = QSettings::value(key, defaultValue);
  value.convert(static_cast<int>(defaultValue.type()));
  return value;
}

void Settings::setStringList(QString const &key, QStringList const &list)
{
  if (list.empty())
    QSettings::remove(key);
  else
    QSettings::setValue(key, list);

  emit settingChanged(key, list);
}

QVariant Settings::getStringList(QString const &key,
                                 QStringList const &defaultValue) const
{
  auto value = QSettings::value(key, defaultValue);
  return value.toStringList();
}

void Settings::signalSettings()
{
  auto keys = QSettings::allKeys();
  for (auto &key : keys)
    emit settingChanged(key, QSettings::value(key));
}
