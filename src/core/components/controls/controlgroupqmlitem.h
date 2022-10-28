// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "controlgroupprofilepart.h"
#include "core/qmlitem.h"
#include <QObject>
#include <string_view>

class QQuickItem;

class ControlGroupQMLItem
: public QMLItem
, public ControlGroupProfilePart::Importer
, public ControlGroupProfilePart::Exporter
{
  Q_OBJECT

 public:
  explicit ControlGroupQMLItem(std::string_view id) noexcept;

 public:
  void activate(bool active) override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;
  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  bool provideActive() const override;
  void takeActive(bool active) override;

  std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) override;

 private:
  class Initializer;

  QQuickItem *findQQuickItem(Item const &i) const;

  bool active_;
};
