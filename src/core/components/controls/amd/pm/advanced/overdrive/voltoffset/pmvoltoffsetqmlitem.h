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
