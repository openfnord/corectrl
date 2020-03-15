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

#include "core/qmlitem.h"
#include "pmfvstateprofilepart.h"
#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVector>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace AMD {

class PMFVStateQMLItem
: public QMLItem
, public AMD::PMFVStateProfilePart::Importer
, public AMD::PMFVStateProfilePart::Exporter
{
  Q_OBJECT

 public:
  explicit PMFVStateQMLItem() noexcept;

 signals:
  void voltRangeChanged(int min, int max);
  void gpuFreqRangeChanged(int min, int max);
  void memFreqRangeChanged(int min, int max);
  void gpuVoltModeChanged(QString const &mode);
  void memVoltModeChanged(QString const &mode);
  void gpuStatesChanged(QVariantList const &states);
  void memStatesChanged(QVariantList const &states);
  void gpuActiveStatesChanged(QVector<int> const &states);
  void memActiveStatesChanged(QVector<int> const &states);
  void gpuStateChanged(int index, int freq, int volt);
  void memStateChanged(int index, int freq, int volt);

 public slots:
  void changeGPUVoltMode(QString const &mode);
  void changeMemVoltMode(QString const &mode);
  void changeGPUState(int index, int freq, int volt);
  void changeMemState(int index, int freq, int volt);
  void changeGPUActiveState(int index, bool activate);
  void changeMemActiveState(int index, bool activate);

 public:
  void activate(bool active) override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  void takeActive(bool active) override;
  bool provideActive() const override;

  void takePMFVStateGPUVoltMode(std::string const &mode) override;
  std::string const &providePMFVStateGPUVoltMode() const override;

  void takePMFVStateMemVoltMode(std::string const &mode) override;
  std::string const &providePMFVStateMemVoltMode() const override;

  void takePMFVStateGPUStates(
      std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                             units::voltage::millivolt_t>> const &states) override;
  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMFVStateGPUState(unsigned int index) const override;

  void takePMFVStateMemStates(
      std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                             units::voltage::millivolt_t>> const &states) override;
  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMFVStateMemState(unsigned int index) const override;

  void
  takePMFVStateGPUActiveStates(std::vector<unsigned int> const &indices) override;
  std::vector<unsigned int> providePMFVStateMemActiveStates() const override;
  void
  takePMFVStateMemActiveStates(std::vector<unsigned int> const &indices) override;
  std::vector<unsigned int> providePMFVStateGPUActiveStates() const override;

  std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) override;

 private:
  class Initializer;

  void voltModes(std::vector<std::string> const &modes);
  void voltRange(units::voltage::millivolt_t min,
                 units::voltage::millivolt_t max);
  void gpuRange(units::frequency::megahertz_t min,
                units::frequency::megahertz_t max);
  void memRange(units::frequency::megahertz_t min,
                units::frequency::megahertz_t max);

  QVector<int> activeStatesIndices(std::vector<unsigned int> const &indices) const;

  bool active_;

  std::string gpuVoltMode_;
  std::string memVoltMode_;

  std::map<unsigned int,
           std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      gpuStates_;
  std::map<unsigned int,
           std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      memStates_;

  std::vector<unsigned int> gpuActiveStates_;
  std::vector<unsigned int> memActiveStates_;

  static bool register_();
  static bool const registered_;
};

} // namespace AMD
