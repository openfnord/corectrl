// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "controlmodeprofilepart.h"
#include "core/qmlitem.h"
#include <QList>
#include <QObject>
#include <QString>
#include <string>
#include <string_view>
#include <vector>

class ControlModeQMLItem
: public QMLItem
, public ControlModeProfilePart::Importer
, public ControlModeProfilePart::Exporter
{
  Q_OBJECT

 public:
  explicit ControlModeQMLItem(std::string_view id) noexcept;

 signals:
  void modeChanged(QString const &mode);
  void modesChanged(QList<QString> const &modes);

 public slots:
  void changeMode(QString const &mode);

 public:
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;
  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  bool provideActive() const override;
  std::string const &provideMode() const override;

  void takeActive(bool active) override;
  void takeMode(std::string const &mode) override;

  std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) override;

  void activate(bool active) override;

 private:
  class Initializer;
  void takeModes(std::vector<std::string> const &modes);

  bool active_;
  std::string mode_;
};
