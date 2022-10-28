// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
