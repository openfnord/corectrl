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
#include "pmvoltcurveprofilepart.h"
#include <QObject>
#include <QPointF>
#include <QString>
#include <QVariantList>
#include <string>
#include <utility>
#include <vector>

namespace AMD {

class PMVoltCurveQMLItem
: public QMLItem
, public AMD::PMVoltCurveProfilePart::Importer
, public AMD::PMVoltCurveProfilePart::Exporter
{
  Q_OBJECT

 public:
  explicit PMVoltCurveQMLItem() noexcept;

 signals:
  void modeChanged(QString const &mode);
  void pointsRangeChanged(int freqMin, int freqMax, int voltMin, int voltMax);
  void pointsChanged(QVariantList const &points);

 public slots:
  void changeMode(QString const &mode);
  void updatePoint(QPointF const &oldPoint, QPointF const &newPoint);

 public:
  void activate(bool active) override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  void takeActive(bool active) override;
  bool provideActive() const override;

  void takePMVoltCurveMode(std::string const &mode) override;
  std::string const &providePMVoltCurveMode() const override;

  void takePMVoltCurvePoints(
      std::vector<std::pair<units::frequency::megahertz_t,
                            units::voltage::millivolt_t>> const &points) override;
  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMVoltCurvePoint(unsigned int index) const override;

  std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) override;

 private:
  class Initializer;

  void modes(std::vector<std::string> const &modes);
  void pointsRange(
      std::vector<std::pair<
          std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
          std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const
          &ranges);

  bool active_;
  std::string mode_;

  QVariantList qPoints_;
  std::vector<std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      points_;

  static bool register_();
  static bool const registered_;
};

} // namespace AMD
