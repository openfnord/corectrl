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
