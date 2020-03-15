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
