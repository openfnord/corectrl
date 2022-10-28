// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/qmlitem.h"
#include "igpuprofilepart.h"
#include <QObject>
#include <string>

class QQuickItem;

class GPUQMLItem
: public QMLItem
, public IGPUProfilePart::Importer
, public IGPUProfilePart::Exporter
{
  Q_OBJECT
  Q_PROPERTY(int index READ provideIndex)

 public:
  void activate(bool active) override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;
  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  bool provideActive() const override;
  int provideIndex() const override;
  std::string const &provideDeviceID() const override;
  std::string const &provideRevision() const override;
  std::optional<std::string> provideUniqueID() const override;

  void takeActive(bool active) override;
  void takeIndex(int index) override;
  void takeDeviceID(std::string const &deviceID) override;
  void takeRevision(std::string const &revision) override;
  void takeUniqueID(std::optional<std::string> uniqueID) override;

  std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) override;

 signals:
  void newGraphItem(QQuickItem *item);

 private:
  class Initializer;

  bool active_;
  std::string deviceID_;
  std::string revision_;
  std::optional<std::string> uniqueID_;
  int index_;

  static bool register_();
  static bool const registered_;
};
