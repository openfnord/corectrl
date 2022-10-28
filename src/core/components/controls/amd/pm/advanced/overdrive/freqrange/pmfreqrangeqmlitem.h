// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/qmlitem.h"
#include "pmfreqrangeprofilepart.h"
#include <QObject>
#include <QString>
#include <QVariantList>
#include <map>
#include <string>
#include <vector>

namespace AMD {

class PMFreqRangeQMLItem
: public QMLItem
, public AMD::PMFreqRangeProfilePart::Importer
, public AMD::PMFreqRangeProfilePart::Exporter
{
  Q_OBJECT
  Q_PROPERTY(QString instanceID READ instanceID)

 public:
  explicit PMFreqRangeQMLItem() noexcept;

 signals:
  void controlLabelChanged(QString const &label);
  void stateRangeChanged(int min, int max);
  void statesChanged(QVariantList const &states);
  void stateChanged(int index, int freq);

 public slots:
  void changeState(int index, int freq);

 public:
  QString const &instanceID() const;

  void activate(bool active) override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  void takeActive(bool active) override;
  void takePMFreqRangeControlName(std::string const &mode) override;
  void takePMFreqRangeStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states) override;

  bool provideActive() const override;
  units::frequency::megahertz_t
  providePMFreqRangeState(unsigned int index) const override;

  std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) override;

 private:
  class Initializer;

  void controlName(std::string const &name);
  void stateRange(units::frequency::megahertz_t min,
                  units::frequency::megahertz_t max);

  QString instanceID_;
  bool active_;
  std::map<unsigned int, units::frequency::megahertz_t> states_;

  static bool register_();
  static bool const registered_;

  static char const *const trStrings[];
};

} // namespace AMD
