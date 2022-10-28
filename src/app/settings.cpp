// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
