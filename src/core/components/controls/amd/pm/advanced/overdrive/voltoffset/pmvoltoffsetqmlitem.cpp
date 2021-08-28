//
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>
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
#include "pmvoltoffsetqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmvoltoffset.h"
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtQml>
#include <memory>

class AMD::PMVoltOffsetQMLItem::Initializer final
: public QMLItem::Initializer
, public AMD::PMVoltOffset::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              AMD::PMVoltOffsetQMLItem &qmlItem) noexcept
  : QMLItem::Initializer(qmlComponentFactory, qmlEngine)
  , outer_(qmlItem)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMVoltOffsetValue(units::voltage::millivolt_t value) override;
  void takePMVoltOffsetRange(units::voltage::millivolt_t min,
                             units::voltage::millivolt_t max) override;

 private:
  AMD::PMVoltOffsetQMLItem &outer_;
};

void AMD::PMVoltOffsetQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

void AMD::PMVoltOffsetQMLItem::Initializer::takePMVoltOffsetValue(
    units::voltage::millivolt_t value)
{
  outer_.takePMVoltOffsetValue(value);
}

void AMD::PMVoltOffsetQMLItem::Initializer::takePMVoltOffsetRange(
    units::voltage::millivolt_t min, units::voltage::millivolt_t max)
{
  outer_.takePMVoltOffsetRange(min, max);
}

AMD::PMVoltOffsetQMLItem::PMVoltOffsetQMLItem() noexcept
{
  setName(tr(AMD::PMVoltOffset::ItemID.data()));
}

void AMD::PMVoltOffsetQMLItem::changeValue(int value)
{
  if (value_ != value) {
    value_ = value;
    emit valueChanged(value_);
    emit settingsChanged();
  }
}

void AMD::PMVoltOffsetQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMVoltOffsetQMLItem::provideImporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMVoltOffsetQMLItem::provideExporter(Item const &)
{
  return {};
}

bool AMD::PMVoltOffsetQMLItem::provideActive() const
{
  return active_;
}

units::voltage::millivolt_t AMD::PMVoltOffsetQMLItem::providePMVoltOffsetValue() const
{
  return units::voltage::millivolt_t(value_);
}

void AMD::PMVoltOffsetQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

void AMD::PMVoltOffsetQMLItem::takePMVoltOffsetValue(
    units::voltage::millivolt_t value)
{
  auto newValue = value.to<int>();
  if (value_ != newValue) {
    value_ = newValue;
    emit valueChanged(value_);
  }
}

std::unique_ptr<Exportable::Exporter> AMD::PMVoltOffsetQMLItem::initializer(
    IQMLComponentFactory const &qmlComponentFactory,
    QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<AMD::PMVoltOffsetQMLItem::Initializer>(
      qmlComponentFactory, qmlEngine, *this);
}

void AMD::PMVoltOffsetQMLItem::takePMVoltOffsetRange(
    units::voltage::millivolt_t min, units::voltage::millivolt_t max)
{
  emit rangeChanged(min.to<int>(), max.to<int>());
}

bool AMD::PMVoltOffsetQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMVoltOffsetQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                              AMD::PMVoltOffset::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMVoltOffset::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(
            &engine, QStringLiteral("qrc:/qml/AMDPMVoltOffsetForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMVoltOffsetQMLItem::registered_ =
    AMD::PMVoltOffsetQMLItem::register_();
