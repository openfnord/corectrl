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
#include "pmfvvoltcurveprofilepart.h"
#include <QObject>
#include <QPointF>
#include <QString>
#include <QVariantList>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace AMD {

class PMFVVoltCurveQMLItem
: public QMLItem
, public AMD::PMFVVoltCurveProfilePart::Importer
, public AMD::PMFVVoltCurveProfilePart::Exporter
{
  Q_OBJECT

 public:
  explicit PMFVVoltCurveQMLItem() noexcept;

 signals:
  void voltRangeChanged(int min, int max);
  void gpuFreqRangeChanged(int min, int max);
  void memFreqRangeChanged(int min, int max);
  void voltModeChanged(QString const &mode);
  void voltCurveChanged(QVariantList const &points);
  void gpuStatesChanged(QVariantList const &states);
  void memStatesChanged(QVariantList const &states);
  void gpuStateChanged(int index, int freq);
  void memStateChanged(int index, int freq);

 public slots:
  void changeVoltMode(QString const &mode);
  void changeGPUState(int index, int freq);
  void changeMemState(int index, int freq);
  void updateVoltCurvePoint(QPointF const &oldPoint, QPointF const &newPoint);

 public:
  void activate(bool active) override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  void takeActive(bool active) override;
  bool provideActive() const override;

  void takePMFVVoltCurveVoltMode(std::string const &mode) override;
  std::string const &providePMFVVoltCurveVoltMode() const override;

  void takePMFVVoltCurveVoltCurve(
      std::vector<std::pair<units::frequency::megahertz_t,
                            units::voltage::millivolt_t>> const &curve) override;
  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMFVVoltCurveVoltCurvePoint(unsigned int index) const override;

  void takePMFVVoltCurveGPUStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states) override;
  units::frequency::megahertz_t
  providePMFVVoltCurveGPUState(unsigned int index) const override;

  void takePMFVVoltCurveMemStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states) override;
  units::frequency::megahertz_t
  providePMFVVoltCurveMemState(unsigned int index) const override;

  std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) override;

 private:
  class Initializer;

  void voltModes(std::vector<std::string> const &modes);
  void voltCurveRange(
      std::vector<std::pair<
          std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
          std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const
          &pointRanges);
  void voltRange(units::voltage::millivolt_t min,
                 units::voltage::millivolt_t max);
  void gpuRange(units::frequency::megahertz_t min,
                units::frequency::megahertz_t max);
  void memRange(units::frequency::megahertz_t min,
                units::frequency::megahertz_t max);

  bool active_;

  std::string voltMode_;

  QVariantList qVoltCurve_;
  std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      voltCurve_;

  std::map<unsigned int, units::frequency::megahertz_t> gpuStates_;
  std::map<unsigned int, units::frequency::megahertz_t> memStates_;

  static bool register_();
  static bool const registered_;
};

} // namespace AMD
