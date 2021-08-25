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
