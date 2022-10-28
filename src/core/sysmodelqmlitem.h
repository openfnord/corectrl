// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
