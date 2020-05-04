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
#include "fanautoqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "fanauto.h"
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtGlobal>
#include <QtQml>
#include <memory>

char const *const AMD::FanAutoQMLItem::trStrings[] = {
    QT_TRANSLATE_NOOP("ControlModeQMLItem", "AMD_FAN_AUTO"),
};

class AMD::FanAutoQMLItem::Initializer final
: public QMLItem::Initializer
, public IControl::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              AMD::FanAutoQMLItem &qmlItem) noexcept
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

 private:
  AMD::FanAutoQMLItem &outer_;
};

void AMD::FanAutoQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

AMD::FanAutoQMLItem::FanAutoQMLItem() noexcept
{
  setName(tr(FanAuto::ItemID.data()));
}

void AMD::FanAutoQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::FanAutoQMLItem::provideImporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::FanAutoQMLItem::provideExporter(Item const &)
{
  return {};
}

bool AMD::FanAutoQMLItem::provideActive() const
{
  return active_;
}

void AMD::FanAutoQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

std::unique_ptr<Exportable::Exporter>
AMD::FanAutoQMLItem::initializer(IQMLComponentFactory const &qmlComponentFactory,
                                 QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<AMD::FanAutoQMLItem::Initializer>(qmlComponentFactory,
                                                            qmlEngine, *this);
}

bool AMD::FanAutoQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::FanAutoQMLItem>("Radman.UIComponents", 1, 0,
                                         AMD::FanAuto::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::FanAuto::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(&engine,
                                QStringLiteral("qrc:/qml/AMDFanAutoForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::FanAutoQMLItem::registered_ = AMD::FanAutoQMLItem::register_();
