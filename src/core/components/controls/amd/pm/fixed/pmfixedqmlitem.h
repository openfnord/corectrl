// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/qmlitem.h"
#include "pmfixedprofilepart.h"
#include <QList>
#include <QObject>
#include <QString>
#include <string>
#include <vector>

namespace AMD {

class PMFixedQMLItem
: public QMLItem
, public AMD::PMFixedProfilePart::Importer
, public AMD::PMFixedProfilePart::Exporter
{
  Q_OBJECT

 public:
  explicit PMFixedQMLItem() noexcept;

 signals:
  void modeChanged(QString const &mode);
  void modesChanged(QList<QString> const &modes);

 public slots:
  void changeMode(QString const &mode);

 public:
  void activate(bool active) override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;
  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  bool provideActive() const override;
  std::string const &providePMFixedMode() const override;

  void takeActive(bool active) override;
  void takePMFixedMode(std::string const &mode) override;

  std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) override;

 private:
  class Initializer;
  void takePMFixedModes(std::vector<std::string> const &modes);

  bool active_;
  std::string mode_;

  static bool register_();
  static bool const registered_;

  static char const *const trStrings[];
};

} // namespace AMD
