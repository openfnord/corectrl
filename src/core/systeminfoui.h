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
