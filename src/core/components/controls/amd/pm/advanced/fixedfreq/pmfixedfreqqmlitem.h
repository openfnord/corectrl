// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
