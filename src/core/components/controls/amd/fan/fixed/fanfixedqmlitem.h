// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/qmlitem.h"
#include "fanfixedprofilepart.h"
#include <QObject>

namespace AMD {

class FanFixedQMLItem
: public QMLItem
, public AMD::FanFixedProfilePart::Importer
, public AMD::FanFixedProfilePart::Exporter
{
  Q_OBJECT

 public:
  explicit FanFixedQMLItem() noexcept;

 signals:
  void valueChanged(int value);
  void fanStopChanged(bool enabled);
  void fanStartValueChanged(int value);

 public slots:
  void changeValue(int value);
  void enableFanStop(bool enabled);
  void changeFanStartValue(int value);

 public:
  void activate(bool active) override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;
  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  bool provideActive() const override;
  units::concentration::percent_t provideFanFixedValue() const override;
  bool provideFanFixedFanStop() const override;
  units::concentration::percent_t provideFanFixedFanStartValue() const override;

  void takeActive(bool active) override;
  void takeFanFixedValue(units::concentration::percent_t value) override;
  void takeFanFixedFanStop(bool enabled) override;
  void takeFanFixedFanStartValue(units::concentration::percent_t value) override;

  std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) override;

 private:
  class Initializer;

  bool active_;

  int value_;
  bool fanStop_;
  int fanStartValue_;

  static bool register_();
  static bool const registered_;

  static char const *const trStrings[];
};

} // namespace AMD
