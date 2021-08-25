//
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>
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

#include "core/qmlitem.h"
#include "pmfreqvoltprofilepart.h"
#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVector>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace AMD {

class PMFreqVoltQMLItem
: public QMLItem
, public AMD::PMFreqVoltProfilePart::Importer
, public AMD::PMFreqVoltProfilePart::Exporter
{
  Q_OBJECT
  Q_PROPERTY(QString instanceID READ instanceID)

 public:
  explicit PMFreqVoltQMLItem() noexcept;

 signals:
  void controlLabelChanged(QString const &label);
  void voltRangeChanged(int min, int max);
  void freqRangeChanged(int min, int max);
  void voltModeChanged(QString const &mode);
  void statesChanged(QVariantList const &states);
  void activeStatesChanged(QVector<int> const &states);
  void stateChanged(int index, int freq, int volt);

 public slots:
  void changeVoltMode(QString const &mode);
  void changeState(int index, int freq, int volt);
  void changeActiveState(int index, bool activate);

 public:
  QString const &instanceID() const;

  void activate(bool active) override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  void takeActive(bool active) override;
  void takePMFreqVoltControlName(std::string const &mode) override;
  void takePMFreqVoltVoltMode(std::string const &mode) override;
  void takePMFreqVoltStates(
      std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                             units::voltage::millivolt_t>> const &states) override;
  void
  takePMFreqVoltActiveStates(std::vector<unsigned int> const &states) override;

  bool provideActive() const override;
  std::string const &providePMFreqVoltVoltMode() const override;
  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMFreqVoltState(unsigned int index) const override;
  std::vector<unsigned int> providePMFreqVoltActiveStates() const override;

  std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) override;

 private:
  class Initializer;

  void controlName(std::string const &name);
  void voltModes(std::vector<std::string> const &modes);
  void voltRange(units::voltage::millivolt_t min,
                 units::voltage::millivolt_t max);
  void freqRange(units::frequency::megahertz_t min,
                 units::frequency::megahertz_t max);

  QVector<int> activeStatesIndices(std::vector<unsigned int> const &indices) const;

  QString instanceID_;
  bool active_;
  std::string voltMode_;
  std::map<unsigned int,
           std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      states_;
  std::vector<unsigned int> activeStates_;

  static bool register_();
  static bool const registered_;

  static char const *const trStrings[];
};

} // namespace AMD
