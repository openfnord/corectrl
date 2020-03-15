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
#include "pmfreqodprofilepart.h"
#include <QObject>
#include <QString>

namespace AMD {

class PMFreqOdQMLItem
: public QMLItem
, public AMD::PMFreqOdProfilePart::Importer
, public AMD::PMFreqOdProfilePart::Exporter
{
  Q_OBJECT

 public:
  explicit PMFreqOdQMLItem() noexcept;

 signals:
  void sclkOdChanged(unsigned int value);
  void mclkOdChanged(unsigned int value);
  void sclkChanged(QString const &value);
  void mclkChanged(QString const &value);

 public slots:
  void changeSclkOd(unsigned int value);
  void changeMclkOd(unsigned int value);

 public:
  void activate(bool active) override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;
  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  bool provideActive() const override;
  void takeActive(bool active) override;

  unsigned int providePMFreqOdSclkOd() const override;
  unsigned int providePMFreqOdMclkOd() const override;

  void takePMFreqOdSclkOd(unsigned int value) override;
  void takePMFreqOdMclkOd(unsigned int value) override;

  std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) override;

 private:
  class Initializer;

  void takePMFreqOdBaseSclk(units::frequency::megahertz_t value);
  void takePMFreqOdBaseMclk(units::frequency::megahertz_t value);

  QString stateLabel(unsigned int value);

  unsigned int sclkOd() const;
  void sclkOd(unsigned int value);

  unsigned int mclkOd() const;
  void mclkOd(unsigned int value);

  bool active_;

  unsigned int sclkOd_{0};
  unsigned int mclkOd_{0};

  unsigned int baseSclk_{0};
  unsigned int baseMclk_{0};

  static bool register_();
  static bool const registered_;
};

} // namespace AMD
