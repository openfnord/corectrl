// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/qmlitem.h"
#include "pmpowercapprofilepart.h"
#include <QObject>
#include <string>

namespace AMD {

class PMPowerCapQMLItem
: public QMLItem
, public AMD::PMPowerCapProfilePart::Importer
, public AMD::PMPowerCapProfilePart::Exporter
{
  Q_OBJECT

 public:
  explicit PMPowerCapQMLItem() noexcept;

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
  units::power::watt_t providePMPowerCapValue() const override;

  void takeActive(bool active) override;
  void takePMPowerCapValue(units::power::watt_t value) override;

  std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) override;

 private:
  void takePMPowerCapRange(units::power::watt_t min, units::power::watt_t max);

  class Initializer;

  bool active_;
  int value_;

  static bool register_();
  static bool const registered_;
};

} // namespace AMD
