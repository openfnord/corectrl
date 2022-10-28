// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QVariant>

class Settings final : public QSettings
{
  Q_OBJECT

 public:
  Settings(QString const &appName) noexcept;

  Q_INVOKABLE void setValue(QString const &key, QVariant const &value);
  Q_INVOKABLE QVariant getValue(QString const &key,
                                QVariant const &defaultValue) const;

  Q_INVOKABLE void setStringList(QString const &key, QStringList const &list);
  Q_INVOKABLE QVariant getStringList(QString const &key,
                                     QStringList const &defaultList) const;

  void signalSettings();

 signals:
  void settingChanged(QString const &key, QVariant const &value);
};
