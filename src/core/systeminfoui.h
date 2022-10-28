// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <string_view>
#include <utility>
#include <vector>

class ISysModel;

class SystemInfoUI : public QObject
{
  Q_OBJECT

 public:
  static constexpr std::string_view QMLComponentID{"SYSTEM_INFO"};

  explicit SystemInfoUI(QObject *parent = nullptr) noexcept;

  void init(ISysModel const *sysModel);

  Q_INVOKABLE void copyToClipboard() const;

 signals:
  void addSystemInfo(QString const &componentName, QVariantList const &info);

 private:
  void initInfo();

  ISysModel const *sysModel_;
  std::vector<std::pair<QString, std::vector<std::pair<QString, QString>>>> info_;

  static bool const registered_;
  static char const *const trStrings[];
};
