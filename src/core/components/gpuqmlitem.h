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
#include "igpuprofilepart.h"
#include <QObject>
#include <string>

class QQuickItem;

class GPUQMLItem
: public QMLItem
, public IGPUProfilePart::Importer
, public IGPUProfilePart::Exporter
{
  Q_OBJECT
  Q_PROPERTY(int index READ provideIndex)

 public:
  void activate(bool active) override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;
  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  bool provideActive() const override;
  int provideIndex() const override;
  std::string const &provideDeviceID() const override;
  std::string const &provideRevision() const override;

  void takeActive(bool active) override;
  void takeIndex(int index) override;
  void takeDeviceID(std::string const &deviceID) override;
  void takeRevision(std::string const &revision) override;

  std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) override;

 signals:
  void newGraphItem(QQuickItem *item);

 private:
  class Initializer;

  bool active_;
  std::string deviceID_;
  std::string revision_;
  int index_;

  static bool register_();
  static bool const registered_;
};
