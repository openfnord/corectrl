// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
