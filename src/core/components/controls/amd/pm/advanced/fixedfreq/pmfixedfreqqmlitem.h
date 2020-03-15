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
#include "pmfixedfreqprofilepart.h"
#include <QObject>
#include <QString>
#include <QVariantList>
#include <utility>
#include <vector>

namespace AMD {

class PMFixedFreqQMLItem
: public QMLItem
, public AMD::PMFixedFreqProfilePart::Importer
, public AMD::PMFixedFreqProfilePart::Exporter
{
  Q_OBJECT

 public:
  explicit PMFixedFreqQMLItem() noexcept;

 signals:
  void sclkIndexChanged(unsigned int index);
  void mclkIndexChanged(unsigned int index);
  void sclkStatesChanged(QVariantList const &states);
  void mclkStatesChanged(QVariantList const &states);

 public slots:
  void changeSclkIndex(unsigned int index);
  void changeMclkIndex(unsigned int index);

 public:
  void activate(bool active) override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;
  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  bool provideActive() const override;
  void takeActive(bool active) override;

  unsigned int providePMFixedFreqSclkIndex() const override;
  unsigned int providePMFixedFreqMclkIndex() const override;

  void takePMFixedFreqSclkIndex(unsigned int index) override;
  void takePMFixedFreqMclkIndex(unsigned int index) override;

  std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) override;

 private:
  class Initializer;

  void takePMFixedFreqSclkStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states);
  void takePMFixedFreqMclkStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states);

  QString stateLabel(unsigned int value);

  unsigned int sclkIndex() const;
  void sclkIndex(unsigned int index);

  unsigned int mclkIndex() const;
  void mclkIndex(unsigned int index);

  bool active_;

  unsigned int sclkIndex_;
  unsigned int mclkIndex_;

  static bool register_();
  static bool const registered_;

  static char const *const trStrings[];
};

} // namespace AMD
