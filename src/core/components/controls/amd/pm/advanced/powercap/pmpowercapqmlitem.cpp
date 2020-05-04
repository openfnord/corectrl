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
#include "pmpowercapqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmpowercap.h"
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtQml>
#include <memory>

class AMD::PMPowerCapQMLItem::Initializer final
: public QMLItem::Initializer
, public AMD::PMPowerCap::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              AMD::PMPowerCapQMLItem &qmlItem) noexcept
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
  void takePMPowerCapValue(units::power::watt_t value) override;
  void takePMPowerCapRange(units::power::watt_t min,
                           units::power::watt_t max) override;

 private:
  AMD::PMPowerCapQMLItem &outer_;
};

void AMD::PMPowerCapQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

void AMD::PMPowerCapQMLItem::Initializer::takePMPowerCapValue(
    units::power::watt_t value)
{
  outer_.takePMPowerCapValue(value);
}

void AMD::PMPowerCapQMLItem::Initializer::takePMPowerCapRange(
    units::power::watt_t min, units::power::watt_t max)
{
  outer_.takePMPowerCapRange(min, max);
}

AMD::PMPowerCapQMLItem::PMPowerCapQMLItem() noexcept
{
  setName(tr(AMD::PMPowerCap::ItemID.data()));
}

void AMD::PMPowerCapQMLItem::changeValue(int value)
{
  if (value_ != value) {
    value_ = value;
    emit valueChanged(value_);
    emit settingsChanged();
  }
}

void AMD::PMPowerCapQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMPowerCapQMLItem::provideImporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMPowerCapQMLItem::provideExporter(Item const &)
{
  return {};
}

bool AMD::PMPowerCapQMLItem::provideActive() const
{
  return active_;
}

units::power::watt_t AMD::PMPowerCapQMLItem::providePMPowerCapValue() const
{
  return units::power::watt_t(value_);
}

void AMD::PMPowerCapQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

void AMD::PMPowerCapQMLItem::takePMPowerCapValue(units::power::watt_t value)
{
  auto newValue = value.to<int>();
  if (value_ != newValue) {
    value_ = newValue;
    emit valueChanged(value_);
  }
}

std::unique_ptr<Exportable::Exporter> AMD::PMPowerCapQMLItem::initializer(
    IQMLComponentFactory const &qmlComponentFactory,
    QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<AMD::PMPowerCapQMLItem::Initializer>(
      qmlComponentFactory, qmlEngine, *this);
}

void AMD::PMPowerCapQMLItem::takePMPowerCapRange(units::power::watt_t min,
                                                 units::power::watt_t max)
{
  emit rangeChanged(min.to<int>(), max.to<int>());
}

bool AMD::PMPowerCapQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMPowerCapQMLItem>("Radman.UIComponents", 1, 0,
                                            AMD::PMPowerCap::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMPowerCap::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(
            &engine, QStringLiteral("qrc:/qml/AMDPMPowerCapForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMPowerCapQMLItem::registered_ =
    AMD::PMPowerCapQMLItem::register_();
