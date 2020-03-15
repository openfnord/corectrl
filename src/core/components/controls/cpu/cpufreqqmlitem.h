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
#include "cpufreqprofilepart.h"
#include <QList>
#include <QObject>
#include <QString>
#include <string>
#include <vector>

class CPUFreqQMLItem
: public QMLItem
, public CPUFreqProfilePart::Importer
, public CPUFreqProfilePart::Exporter
{
  Q_OBJECT

 public:
  explicit CPUFreqQMLItem() noexcept;

 signals:
  void scalingGovernorChanged(QString const &governor);
  void scalingGovernorsChanged(QList<QString> const &governors);

 public slots:
  void changeScalingGovernor(QString const &governor);

 public:
  void activate(bool active) override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;
  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  bool provideActive() const override;
  std::string const &provideCPUFreqScalingGovernor() const override;

  void takeActive(bool active) override;
  void takeCPUFreqScalingGovernor(std::string const &governor) override;

  std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) override;

 private:
  class Initializer;
  void takeCPUFreqScalingGovernors(std::vector<std::string> const &governors);

  bool active_;
  std::string scalingGovernor_;

  static bool register_();
  static bool const registered_;

  static char const *const trStrings[];
};
