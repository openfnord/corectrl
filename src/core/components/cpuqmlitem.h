// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/qmlitem.h"
#include "icpuprofilepart.h"
#include <QObject>

class QQuickItem;

class CPUQMLItem
: public QMLItem
, public ICPUProfilePart::Importer
, public ICPUProfilePart::Exporter
{
  Q_OBJECT
  Q_PROPERTY(int socketId READ provideSocketId)

 public:
  void activate(bool active) override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;
  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  bool provideActive() const override;
  int provideSocketId() const override;

  void takeActive(bool active) override;
  void takeSocketId(int id) override;

  std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) override;

 signals:
  void newGraphItem(QQuickItem *item);

 private:
  class Initializer;

  bool active_;
  int socketId_;

  static bool register_();
  static bool const registered_;
};
