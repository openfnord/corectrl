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

#include "exportable.h"
#include "iprofile.h"
#include "isysmodelui.h"
#include "qmlitem.h"
#include <QObject>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

class IQMLComponentFactory;
class QQmlApplicationEngine;

class SysModelQMLItem
: public QMLItem
, public ISysModelUI
{
  Q_OBJECT

 public:
  static constexpr std::string_view ParentObjectName{"PROFILE_SYS_MODEL"};

  void activate(bool active) override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;
  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  bool provideActive() const override;
  IProfile::Info const &provideInfo() const override;

  void takeActive(bool active) override;
  void takeInfo(IProfile::Info const &info) override;

  std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) override;

 private:
  class Initializer;

  std::unordered_map<std::string, QMLItem *> components_;

  IProfile::Info profileInfo_;
  bool profileActive_;

  static bool register_();
  static bool const registered_;
};
