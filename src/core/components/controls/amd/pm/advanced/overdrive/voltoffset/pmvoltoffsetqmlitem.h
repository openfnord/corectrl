// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/qmlitem.h"
#include "pmvoltoffsetprofilepart.h"
#include <QObject>
#include <string>

namespace AMD {

class PMVoltOffsetQMLItem
: public QMLItem
, public AMD::PMVoltOffsetProfilePart::Importer
, public AMD::PMVoltOffsetProfilePart::Exporter
{
  Q_OBJECT

 public:
  explicit PMVoltOffsetQMLItem() noexcept;

 signals:
  void valueChanged(int value);
  void rangeChanged(int min, int max);

 public slots:
  void changeValue(int value);

 public:
  void activate(bool active) override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;
  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  bool provideActive() const override;
  units::voltage::millivolt_t providePMVoltOffsetValue() const override;

  void takeActive(bool active) override;
  void takePMVoltOffsetValue(units::voltage::millivolt_t value) override;

  std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) override;

 private:
  void takePMVoltOffsetRange(units::voltage::millivolt_t min,
                             units::voltage::millivolt_t max);

  class Initializer;

  bool active_;
  int value_;

  static bool register_();
  static bool const registered_;
};

} // namespace AMD
