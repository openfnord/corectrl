// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/qmlitem.h"
#include "noopprofilepart.h"
#include <QObject>

class NoopQMLItem
: public QMLItem
, public NoopProfilePart::Importer
, public NoopProfilePart::Exporter
{
  Q_OBJECT

 public:
  explicit NoopQMLItem() noexcept;

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

  bool active_;

  static bool register_();
  static bool const registered_;

  static char const *const trStrings[];
};
